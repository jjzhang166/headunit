#ifndef HU_GST_H
#define HU_GST_H

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <linux/input.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <poll.h>

#include "hu_uti.h"
#include "hu_aap.h"

#define EVENT_DEVICE_TS "/dev/input/filtered-touchscreen0"
#define EVENT_DEVICE_CMD "/dev/input/event1"
#define EVENT_TYPE EV_ABS
#define EVENT_CODE_X ABS_X
#define EVENT_CODE_Y ABS_Y

#define ACTION_DOWN 0
#define ACTION_UP 1
#define ACTION_MOVE 2
#define TS_MAX_REQ_SZ 32

__asm__(".symver realpath1,realpath1@GLIBC_2.11.1");

typedef struct {
  GMainLoop *loop;
  GstPipeline *pipeline;
  GstPipeline *musicpipeline;
  GstPipeline *voicepipeline;
  GstPipeline *micpipeline;
  GstElement *audio;
  GstAppSrc *src;
  GstAppSrc *musicsrc;
  GstAppSrc *voicesrc;
  GstElement *sink;
  GstElement *micsink;
  guint sourceid;
} gst_app_t;

typedef struct {
  int fd;
  int x;
  int y;
  uint8_t action;
  int action_recvd;
} mytouchscreen;

typedef struct {
  int fd;
  uint8_t action;
} mycommander;

typedef struct {
  int retry;
  int chan;
  int cmd_len;
  unsigned char *cmd_buf;
  int shouldFree;
} send_arg;

typedef enum {
  HUIB_UP = 0x13,
  HUIB_DOWN = 0x14,
  HUIB_LEFT = 0x15,
  HUIB_RIGHT = 0x16,
  HUIB_BACK = 0x04,
  HUIB_ENTER = 0x17,
  HUIB_MIC = 0x54,
  HUIB_PLAYPAUSE = 0x55,
  HUIB_NEXT = 0x57,
  HUIB_PREV = 0x58,
  HUIB_PHONE = 0x5,
  HUIB_START = 126,
  HUIB_STOP = 127,

} HU_INPUT_BUTTON;

typedef struct {
    bool mic_enabled;
    char alsa_input[1024];
    char alsa_output[1024];
} hu_gst_settings_t;


static const uint8_t ts_header[] = {0x80, 0x01, 0x08};
static const uint8_t ts_sizes[] = {0x1a, 0x09, 0x0a, 0x03};
static const uint8_t ts_footer[] = {0x10, 0x00, 0x18};
static const uint8_t mic_header[] = {0x00, 0x00};
static const int max_size = 8192;

static int shouldRead = FALSE;
static int shouldReadAudio = FALSE;
static gst_app_t gst_app;


void queueSend(int retry, int chan, unsigned char *cmd_buf, int cmd_len,
               int shouldFree);

static GstFlowReturn read_mic_data(GstElement *sink);
static gboolean read_data(gst_app_t *app);
static void start_feed(GstElement *pipeline, guint size, void *app);
static void stop_feed(GstElement *pipeline, void *app);
static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer *ptr);
static int gst_pipeline_init(gst_app_t *app, void *widget);
static int gst_audio_init(gst_app_t *app);
static size_t uleb128_encode(uint64_t value, uint8_t *data);
static size_t varint_encode(uint64_t val, uint8_t *ba, int idx);
static size_t uptime_encode(uint64_t value, uint8_t *data);
static gboolean delayedShouldDisplayTrue(gpointer data);
static int hu_fill_button_message(uint8_t *buffer, uint64_t timeStamp,
                                  HU_INPUT_BUTTON button, int isPress);
static void handle_button_press(int keycode);
static void on_pad_added(GstElement *element, GstPad *pad);
gboolean myMainLoop(gpointer app);
static void *main_thread(void *app);
static int gst_loop(gst_app_t *app);
static void signals_handler(int signum);

#ifdef __cplusplus
extern "C" {
#endif
int aa_gst(void *widget);
void aa_touch_event(uint8_t action, int x, int y);
void hu_gst_set_settings(bool mic_enabled,char *alsa_input,char *alsa_output);
#ifdef __cplusplus
}
#endif

#endif // HU_GST_H
