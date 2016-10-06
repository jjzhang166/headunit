#include "hu_gst.h"

int nightmode = 0;
int displayStatus = 1;
int mic_change_state = 0;

GstElement *mic_pipeline, *mic_sink;
GstElement *aud_pipeline, *aud_src;
GstElement *au1_pipeline, *au1_src;

GAsyncQueue *sendqueue;

GMainLoop *mainloop;
mycommander mCommander = {0, 0};
mytouchscreen mTouch = {0, 0, 0, 0, 0};

void queueSend(int retry, int chan, unsigned char *cmd_buf, int cmd_len,
               int shouldFree) {
  send_arg *cmd = malloc(sizeof(send_arg));

  cmd->retry = retry;
  cmd->chan = chan;
  cmd->cmd_buf = cmd_buf;
  cmd->cmd_len = cmd_len;
  cmd->shouldFree = shouldFree;

  g_async_queue_push(sendqueue, cmd);
}

int timestamp = 0, tried = 0, frames_pushed = 0;
double elapsedTime = 0;

static gboolean read_data(gst_app_t *app) {
  GstBuffer *buffer;
  guint8 *ptr;
  GstFlowReturn ret;
  int iret;
  char *vbuf;
  char *abuf;
  int res_len = 0;

  struct timespec t1, t2;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);

  int timenow = (int)time(NULL);

  tried++;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
  elapsedTime += ((double)(t2.tv_sec - t1.tv_sec) * 1.0e9 +
                  (double)(t2.tv_nsec - t1.tv_nsec)) /
                 1000000000;

  iret = hu_aap_recv_process();

  if (iret != 0) {
    g_print("hu_aap_recv_process() iret: %d\n", iret);
    g_main_loop_quit(app->loop);
    return FALSE;
  }

  /* Is there a video buffer queued? */
  vbuf = vid_read_head_buf_get(&res_len);

  if (vbuf != NULL) {
    frames_pushed++;

    buffer = gst_buffer_new_and_alloc(res_len);
    gst_buffer_fill(buffer, 0, vbuf, res_len);

    ret = gst_app_src_push_buffer((GstAppSrc *)app->src, buffer);

    if (ret != GST_FLOW_OK) {
      g_print("push buffer returned %d for %d bytes \n", ret, res_len);
      return FALSE;
    }
  }
  if (timenow != timestamp) {
    g_print(
        "%d: %d FPS (tried %d times) | receive and push time : %lf seconds\n",
        timestamp, frames_pushed, tried, elapsedTime);
    timestamp = timenow;
    frames_pushed = 0;
    tried = 0;
    elapsedTime = 0;
  }
  return TRUE;
}

static void start_feed(GstElement *pipeline, guint size, void *app) {
  shouldRead = TRUE;
}

static void stop_feed(GstElement *pipeline, void *app) { shouldRead = FALSE; }

static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer *ptr) {
  gst_app_t *app = (gst_app_t *)ptr;

  switch (GST_MESSAGE_TYPE(message)) {

  case GST_MESSAGE_ERROR: {
    gchar *debug;
    GError *err;

    gst_message_parse_error(message, &err, &debug);
    g_print("Error %s\n", err->message);
    g_error_free(err);
    g_free(debug);
    g_main_loop_quit(app->loop);
  } break;

  case GST_MESSAGE_WARNING: {
    gchar *debug;
    GError *err;
    gchar *name;

    gst_message_parse_warning(message, &err, &debug);
    g_print("Warning %s\nDebug %s\n", err->message, debug);

    name = (gchar *)GST_MESSAGE_SRC_NAME(message);

    g_print("Name of src %s\n", name ? name : "nil");
    g_error_free(err);
    g_free(debug);
  } break;

  case GST_MESSAGE_EOS:
    g_print("End of stream\n");
    g_main_loop_quit(app->loop);
    break;

  case GST_MESSAGE_STATE_CHANGED:
    break;

  default:
    //					 g_print("got message %s\n", \
        gst_message_type_get_name (GST_MESSAGE_TYPE (message)));
    break;
  }

  return TRUE;
}

static int gst_pipeline_init(gst_app_t *app, void *widget) {
  GstBus *bus;
  GstStateChangeReturn state_ret;

  GError *error = NULL;

  gst_init(NULL, NULL);

  //	app->pipeline = (GstPipeline*)gst_parse_launch("appsrc name=mysrc
  // is-live=true block=false max-latency=1000000 ! h264parse ! vpudec
  // low-latency=true framedrop=true framedrop-level-mask=0x200 ! mfw_v4lsink
  // max-lateness=1000000000 sync=false async=false", &error);

  // app->pipeline = (GstPipeline*)gst_parse_launch("appsrc name=mysrc
  // is-live=true block=false max-latency=1000000 ! h264parse ! vpudec
  // low-latency=true framedrop=true framedrop-level-mask=0x200 ! mfw_isink
  // name=mysink axis-left=25 axis-top=0 disp-width=751 disp-height=480
  // max-lateness=1000000000 sync=false async=false", &error);
  app->pipeline = (GstPipeline *)gst_parse_launch(
      "appsrc name=mysrc is-live=true block=false max-latency=-1 "
      "blocksize=262144 do-timestamp=true max-bytes=262144 "
      "stream-type=stream typefind=true ! "
      "h264parse ! "
      "avdec_h264 lowres=2 output-corrupt=false ! "
      "videoconvert name=videoconvert ! "
      "qwidget5videosink name=mysink sync=false async=false "
      "max-lateness=1000000 blocksize=262144",
      &error);
  if (error != NULL) {
    g_print("could not construct pipeline: %s\n", error->message);
    g_clear_error(&error);
    return -1;
  }

  bus = gst_pipeline_get_bus(app->pipeline);
  gst_bus_add_watch(bus, (GstBusFunc)bus_callback, app);
  gst_object_unref(bus);

  app->src = (GstAppSrc *)gst_bin_get_by_name(GST_BIN(app->pipeline), "mysrc");

  app->sink =
      (GstElement *)gst_bin_get_by_name(GST_BIN(app->pipeline), "mysink");

  g_object_set(G_OBJECT(app->sink), "widget", widget, NULL);

  gst_app_src_set_stream_type(app->src, GST_APP_STREAM_TYPE_STREAM);

  g_signal_connect(app->src, "need-data", G_CALLBACK(start_feed), app);

  g_signal_connect(app->src, "enough-data", G_CALLBACK(stop_feed), app);

  return 0;
}

static size_t uleb128_encode(uint64_t value, uint8_t *data) {
  uint8_t cbyte;
  size_t enc_size = 0;

  do {
    cbyte = value & 0x7f;
    value >>= 7;
    if (value != 0)
      cbyte |= 0x80;
    data[enc_size++] = cbyte;
  } while (value != 0);

  return enc_size;
}

static size_t varint_encode(uint64_t val, uint8_t *ba, int idx) {

  if (val >= 0x7fffffffffffffff) {
    return 1;
  }

  uint64_t left = val;
  int idx2 = 0;

  for (idx2 = 0; idx2 < 9; idx2++) {
    ba[idx + idx2] = (uint8_t)(0x7f & left);
    left = left >> 7;
    if (left == 0) {
      return (idx2 + 1);
    } else if (idx2 < 9 - 1) {
      ba[idx + idx2] |= 0x80;
    }
  }

  return 9;
}

/*
 * Function to send touch coordinates
 * TODO: Implement touch in QT
 */
void aa_touch_event(uint8_t action, int x, int y) {
  struct timespec tp;
  uint8_t *buf;
  int idx;
  int siz_arr = 0;
  int size1_idx, size2_idx, i;
  int axis = 0;
  int coordinates[3] = {x, y, 0};
  int ret;

  buf = (uint8_t *)malloc(TS_MAX_REQ_SZ);
  if (!buf) {
    g_print("Failed to allocate touchscreen event buffer\n");
    return;
  }

  /* Fetch the time stamp */
  clock_gettime(CLOCK_REALTIME, &tp);

  /* Copy header */
  memcpy(buf, ts_header, sizeof(ts_header));
  //	idx = sizeof(ts_header) +
  //	      uleb128_encode(tp.tv_nsec, buf + sizeof(ts_header));

  idx = sizeof(ts_header) + varint_encode(tp.tv_sec * 1000000000 + tp.tv_nsec,
                                          buf + sizeof(ts_header), 0);

  size1_idx = idx + 1;
  size2_idx = idx + 3;

  /* Copy sizes */
  memcpy(buf + idx, ts_sizes, sizeof(ts_sizes));
  idx += sizeof(ts_sizes);

  /* Set magnitude of each axis */
  for (i = 0; i < 3; i++) {
    axis += 0x08;
    buf[idx++] = axis;
    /* FIXME The following can be optimzed to update size1/2 at end of loop */
    siz_arr = uleb128_encode(coordinates[i], &buf[idx]);
    idx += siz_arr;
    buf[size1_idx] += siz_arr;
    buf[size2_idx] += siz_arr;
  }

  /* Copy footer */
  memcpy(buf + idx, ts_footer, sizeof(ts_footer));
  idx += sizeof(ts_footer);

  buf[idx++] = action;

  queueSend(0, AA_CH_TOU, buf, idx, TRUE);
}

static size_t uptime_encode(uint64_t value, uint8_t *data) {

  int ctr = 0;
  for (ctr = 7; ctr >= 0; ctr--) { // Fill 8 bytes backwards
    data[6 + ctr] = (uint8_t)(value & 0xFF);
    value = value >> 8;
  }

  return 8;
}

static void read_mic_data(GstElement *sink) {
  GstBuffer *gstbuf;
  int ret;

  g_signal_emit_by_name(sink, "pull-buffer", &gstbuf, NULL);

  if (gstbuf) {

    struct timespec tp;

    /* if mic is stopped, don't bother sending */

    if (mic_change_state == 0) {
      g_print("Mic stopped.. dropping buffers \n");
      gst_buffer_unref(gstbuf);
      return;
    }

    /* Fetch the time stamp */
    clock_gettime(CLOCK_REALTIME, &tp);

    gint mic_buf_sz;
    mic_buf_sz = gst_buffer_get_size(gstbuf);

    int idx;

    if (mic_buf_sz <= 64) {
      g_print("Mic data < 64 \n");
      return;
    }

    uint8_t *mic_buffer = (uint8_t *)malloc(14 + mic_buf_sz);

    /* Copy header */
    memcpy(mic_buffer, mic_header, sizeof(mic_header));

    idx = sizeof(mic_header) + uptime_encode(tp.tv_nsec * 0.001, mic_buffer);

    /* Copy PCM Audio Data */

    gst_buffer_fill(mic_buffer + idx, 0, gstbuf, mic_buf_sz);
    // memcpy(mic_buffer+idx, GST_BUFFER_DATA(gstbuf), mic_buf_sz);
    idx += mic_buf_sz;

    queueSend(1, AA_CH_MIC, mic_buffer, idx, TRUE);

    gst_buffer_unref(gstbuf);
  }
}

static int hu_fill_button_message(uint8_t *buffer, uint64_t timeStamp,
                                  HU_INPUT_BUTTON button, int isPress) {
  int buffCount = 0;
  buffer[buffCount++] = 0x80;
  buffer[buffCount++] = 0x01;
  buffer[buffCount++] = 0x08;

  buffCount += varint_encode(timeStamp, buffer + buffCount, 0);

  buffer[buffCount++] = 0x22;
  buffer[buffCount++] = 0x0A;
  buffer[buffCount++] = 0x0A;
  buffer[buffCount++] = 0x08;
  buffer[buffCount++] = 0x08;
  buffer[buffCount++] = (uint8_t)button;
  buffer[buffCount++] = 0x10;
  buffer[buffCount++] = isPress ? 0x01 : 0x00;
  buffer[buffCount++] = 0x18;
  buffer[buffCount++] = 0x00;
  buffer[buffCount++] = 0x20;
  buffer[buffCount++] = 0x00;
  return buffCount;
}

/*
 * A function to handle button presses
 * Based on the function from gartnera
 */
// TODO:Define constants for this
// TODO:Implement it with QT
static void handle_button_press(int keycode) {
  gst_app_t *app = &gst_app;

  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  uint64_t timestamp = tp.tv_sec * 1000000000 + tp.tv_nsec;
  uint8_t *keyTempBuffer = 0;
  int keyTempSize = 0;
  switch (keycode) {
  case 1:
    g_print("KEY_G\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_MIC, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  // Make the music button play/pause
  case 2:
    g_print("KEY_E\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_PLAYPAUSE, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 3:
    g_print("KEY_LEFTBRACE\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_NEXT, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 4:
    g_print("KEY_RIGHTBRACE\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_PREV, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 5:
    g_print("KEY_BACKSPACE\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_BACK, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 6:
    g_print("KEY_ENTER\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_ENTER, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 7:
  case 8:
    g_print("KEY_LEFT\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_LEFT, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 9:
  case 10:
    g_print("KEY_RIGHT\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_RIGHT, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 11:
    g_print("KEY_UP\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_UP, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 12:
    g_print("KEY_DOWN\n");
    keyTempBuffer = malloc(512);
    keyTempSize =
        hu_fill_button_message(keyTempBuffer, timestamp, HUIB_DOWN, TRUE);
    queueSend(0, AA_CH_TOU, keyTempBuffer, keyTempSize, TRUE);
    break;
  case 13:
    g_print("KEY_HOME\n");
    if (TRUE) {
      g_main_loop_quit(mainloop);
    }
    break;
  case 14:
    g_print("KEY_R\n");

    break;
  }
}

gboolean myMainLoop(gpointer app) {
  if (shouldRead) {
    read_data(app);
  }

  send_arg *cmd;

  if (cmd = g_async_queue_try_pop(sendqueue)) {
    hu_aap_enc_send(cmd->retry, cmd->chan, cmd->cmd_buf, cmd->cmd_len);
    if (cmd->shouldFree)
      free(cmd->cmd_buf);
    free(cmd);
  }

  return TRUE;
}

static void *main_thread(void *app) {

  ms_sleep(100);

  while (mainloop && g_main_loop_is_running(mainloop)) {
    myMainLoop(app);
  }
}

static int gst_loop(gst_app_t *app) {
  int ret;
  GstStateChangeReturn state_ret;

  state_ret =
      gst_element_set_state((GstElement *)app->pipeline, GST_STATE_PLAYING);
  state_ret =
      gst_element_set_state((GstElement *)aud_pipeline, GST_STATE_PLAYING);
  state_ret =
      gst_element_set_state((GstElement *)au1_pipeline, GST_STATE_PLAYING);

  //	g_warning("set state returned %d\n", state_ret);

  app->loop = g_main_loop_new(NULL, FALSE);

  mainloop = app->loop;

  //	g_timeout_add_full(G_PRIORITY_HIGH, 1, myMainLoop, (gpointer)app, NULL);

  g_print("Starting Android Auto...\n");
  g_main_loop_run(app->loop);

  // TO-DO
  state_ret =
      gst_element_set_state((GstElement *)app->pipeline, GST_STATE_NULL);
  //	g_warning("set state null returned %d\n", state_ret);

  gst_object_unref(app->pipeline);
  gst_object_unref(mic_pipeline);
  gst_object_unref(aud_pipeline);
  gst_object_unref(au1_pipeline);

  return ret;
}

static void signals_handler(int signum) {
  if (signum == SIGINT) {
    if (mainloop && g_main_loop_is_running(mainloop)) {
      g_main_loop_quit(mainloop);
    }
  }
}
int aa_gst(void *widget) {
  signal(SIGTERM, signals_handler);

  gst_app_t *app = &gst_app;
  int ret = 0;
  errno = 0;
  byte ep_in_addr = -2;
  byte ep_out_addr = -2;

  /* Start AA processing */
  ret = hu_aap_start(ep_in_addr, ep_out_addr, 2050074816, 0, 0);
  if (ret == -1) {
    g_print("Phone switched to accessory mode. Attempting once more.\n");
    sleep(1);
    ret = hu_aap_start(ep_in_addr, ep_out_addr, 2050074816, 0, 0);
  }

  if (ret < 0) {
    if (ret == -2) {
      g_print(
          "Phone is not connected. Connect a supported phone and restart.\n");
      return 0;
    } else if (ret == -1)
      g_print("Phone switched to accessory mode. Restart to enter AA mode.\n");
    else
      g_print("hu_app_start() ret: %d\n", ret);
    return (ret);
  }

  g_print("Starting Android Auto...\n");
  /* Init gstreamer pipeline */
  ret = gst_pipeline_init(app, widget);
  if (ret < 0) {
    g_print("gst_pipeline_init() ret: %d\n", ret);
    return (-4);
  }

  /* Open Touchscreen Device */

  sendqueue = g_async_queue_new();

  pthread_t mn_thread;

  pthread_create(&mn_thread, NULL, &main_thread, (void *)app);

  /* Start gstreamer pipeline and main loop */
  ret = gst_loop(app);
  if (ret < 0) {
    g_print("gst_loop() ret: %d\n", ret);
    ret = -5;
  }

  /* Stop AA processing */
  ret = hu_aap_stop();
  if (ret < 0) {
    g_print("hu_aap_stop() ret: %d\n", ret);
    ret = -6;
  }

  pthread_cancel(mn_thread);

  g_print("END \n");

  return 0;
}
