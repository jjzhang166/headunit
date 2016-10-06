// Utilities: Used by many

//#ifndef UTILS_INCLUDED

//  #define UTILS_INCLUDED

//#define  GENERIC_CLIENT

#define LOGTAG "hu_uti"
#include "hu_uti.h"

#ifndef NDEBUG
char *state_get(int state) {
  switch (state) {
  case hu_STATE_INITIAL: // 0
    return ("hu_STATE_INITIAL");
  case hu_STATE_STARTIN: // 1
    return ("hu_STATE_STARTIN");
  case hu_STATE_STARTED: // 2
    return ("hu_STATE_STARTED");
  case hu_STATE_STOPPIN: // 3
    return ("hu_STATE_STOPPIN");
  case hu_STATE_STOPPED: // 4
    return ("hu_STATE_STOPPED");
  }
  return ("hu_STATE Unknown error");
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#include <assert.h>

#include <string.h>
#include <signal.h>

#include <pthread.h>

#include <errno.h>

#include <fcntl.h>

#include "hu_platform_specific.c"

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <WinSock2.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

//#define   S2D_POLL_MS     100
//#define   NET_PORT_S2D    2102
//#define   NET_PORT_HCI    2112

//#include "man_ver.h"

#ifdef WIN32
#ifndef _WINDOWS_
#include <Windows.h>
#endif
LARGE_INTEGER
getFILETIMEoffset_win_fix() {
  SYSTEMTIME s;
  FILETIME f;
  LARGE_INTEGER t;

  s.wYear = 1970;
  s.wMonth = 1;
  s.wDay = 1;
  s.wHour = 0;
  s.wMinute = 0;
  s.wSecond = 0;
  s.wMilliseconds = 0;
  SystemTimeToFileTime(&s, &f);
  t.QuadPart = f.dwHighDateTime;
  t.QuadPart <<= 32;
  t.QuadPart |= f.dwLowDateTime;
  return (t);
}

int clock_gettime_win_fix(struct timeval *tv) {
  LARGE_INTEGER t;
  FILETIME f;
  double microseconds;
  static LARGE_INTEGER offset;
  static double frequencyToMicroseconds;
  static int initialized = 0;
  static BOOL usePerformanceCounter = 0;

  if (!initialized) {
    LARGE_INTEGER performanceFrequency;
    initialized = 1;
    usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
    if (usePerformanceCounter) {
      QueryPerformanceCounter(&offset);
      frequencyToMicroseconds =
          (double)performanceFrequency.QuadPart / 1000000.;
    } else {
      offset = getFILETIMEoffset_win_fix();
      frequencyToMicroseconds = 10.;
    }
  }
  if (usePerformanceCounter)
    QueryPerformanceCounter(&t);
  else {
    GetSystemTimeAsFileTime(&f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
  }

  t.QuadPart -= offset.QuadPart;
  microseconds = (double)t.QuadPart / frequencyToMicroseconds;
  t.QuadPart = microseconds;
  tv->tv_sec = t.QuadPart / 1000000;
  tv->tv_usec = t.QuadPart % 1000000;
  return (0);
}
// Sauce : https://gist.github.com/ngryman/6482577

void usleep(DWORD waitTime) {
  LARGE_INTEGER perfCnt, start, now;

  QueryPerformanceFrequency(&perfCnt);
  QueryPerformanceCounter(&start);

  do {
    QueryPerformanceCounter((LARGE_INTEGER *)&now);

  } while ((now.QuadPart - start.QuadPart) / (float)perfCnt.QuadPart * 1000 *
               1000 <
           waitTime);
}
#endif

// See : http://stackoverflow.com/questions/35750940/ctime-r-on-msvc
const char *my_ctime_r(char *buffer, size_t bufsize, time_t cur_time) {
#ifdef WIN32
  errno_t e = ctime_s(buffer, bufsize, cur_time);
  assert(e == 0 && "Huh? ctime_s returned an error");
  return buffer;
#else
  const char *res = ctime_r(buffer, cur_time);
  assert(res != NULL && "ctime_r failed...");
  return res;
#endif
}
int gen_server_loop_func(unsigned char *cmd_buf, int cmd_len,
                         unsigned char *res_buf, int res_max);
int gen_server_poll_func(int poll_ms);

// Log stuff:

int ena_log_extra = 1;
int ena_log_verbo = 1;
int ena_log_debug = 1;
int ena_log_warni = 1;
int ena_log_error = 1;

int ena_log_aap_send = 0;

// Enables for hex_dump:
int ena_hd_hu_aad_dmp = 1; // Higher level
int ena_hd_tra_send = 1;   // Lower  level
int ena_hd_tra_recv = 0;

int ena_log_hexdu = 1; // 1;    // Hex dump master enable
int max_hex_dump = 64; // 32;

#ifdef LOG_FILE
int logfd = -1;
void logfile(char *log_line) {
  if (logfd < 0)
    logfd =
        open("/sdcard/hulog", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
  int written = -77;
  if (logfd >= 0)
    written = write(logfd, log_line, strlen(log_line));
}
#endif

char *prio_get(int prio) {
  switch (prio) {
  case hu_LOG_EXT:
    return ("X");
  case hu_LOG_VER:
    return ("V");
  case hu_LOG_DEB:
    return ("D");
  case hu_LOG_WAR:
    return ("W");
  case hu_LOG_ERR:
    return ("E");
  }
  return ("?");
}

int hu_log(int prio, const char *tag, const char *func, const char *fmt, ...) {

  if (!ena_log_extra && prio == hu_LOG_EXT)
    return -1;
  if (!ena_log_verbo && prio == hu_LOG_VER)
    return -1;
  if (!ena_log_debug && prio == hu_LOG_DEB)
    return -1;
  if (!ena_log_warni && prio == hu_LOG_WAR)
    return -1;
  if (!ena_log_error && prio == hu_LOG_ERR)
    return -1;

  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  char *timeStr = asctime(timeinfo);
  /* TODO: Do something with hu_log function with variable length argument list
   * keeps crashing on 64-bit (MSVC, GCC [ubuntu, windows])
   * seems to work fine with Clang on ubuntu tho.
   */
  char log_line[4096] = {0};
  va_list aq;
  va_start(aq, fmt);
  int len = vsnprintf(log_line, sizeof(log_line), fmt, aq);
  va_end(aq);

#ifdef WIN32
  char output[4096];
  sprintf(output, "%.24s %s: %s:: %s\n", timeStr, prio_get(prio), tag,
          log_line);
  OutputDebugStringA(output);
#else
  printf("%.24s %s: %s:: %s\n", timeStr, prio_get(prio), tag, log_line);
#endif
#ifdef LOG_FILE
  char log_line[4096] = {0};
  va_list aq;
  va_start(aq, fmt);
  int len = vsnprintf(log_line, sizeof(log_line), fmt, aq);
  strlcat(log_line, "\n", sizeof(log_line));
  logfile(log_line);
#endif
  return (0);
}

/*
  int loge (const char * fmt, ...) {
    printf ("E: ");
    va_list ap;
    va_start (ap, fmt);
    vprintf (fmt, ap);
    printf ("\n");
    return (0);
  }
*/

#define MAX_ITOA_SIZE 32 // Int 2^32 need max 10 characters, 2^64 need 21

char *itoa(int val, char *ret, int radix) {
  if (radix == 10)
    snprintf(ret, MAX_ITOA_SIZE - 1, "%d", val);
  else if (radix == 16)
    snprintf(ret, MAX_ITOA_SIZE - 1, "%x", val);
  else
    loge("radix != 10 && != 16: %d", radix);
  return (ret);
}

unsigned long us_get() { // !!!! Affected by time jumps ?
  struct timespec tspec = {0, 0};
#ifdef WIN32
  int res = clock_gettime_win_fix(&tspec);
#else
  int res = clock_gettime(CLOCK_MONOTONIC, &tspec);
#endif
  // logd ("sec: %ld  nsec: %ld", tspec.tv_sec, tspec.tv_nsec);

  unsigned long microsecs = (tspec.tv_nsec / 1000L);
  microsecs += (tspec.tv_sec * 1000000L);

  return (microsecs);
}

unsigned long ms_get() { // !!!! Affected by time jumps ?
  struct timespec tspec = {0, 0};
#ifdef WIN32
  int res = clock_gettime_win_fix(&tspec);
#else
  int res = clock_gettime(CLOCK_MONOTONIC, &tspec);
#endif
  // logd ("sec: %ld  nsec: %ld", tspec.tv_sec, tspec.tv_nsec);

  unsigned long millisecs = (tspec.tv_nsec / 1000000L);
  millisecs += (tspec.tv_sec * 1000L); // remaining 22 bits good for monotonic
                                       // time up to 4 million seconds =~ 46
                                       // days. (?? - 10 bits for 1024 ??)

  return (millisecs);
}

#define quiet_ms_sleep ms_sleep

unsigned long ms_sleep(unsigned long ms) {
  /*
  usleep (ms * 1000L);
  return (0);
//*/

  struct timespec tm;
  tm.tv_sec = 0;
  tm.tv_sec = ms / 1000L;
  tm.tv_nsec = (ms % 1000L) * 1000000L;
  // printf ("tm.tv_sec: %ld  tm.tv_nsec: %ld\n", tm.tv_sec, tm.tv_nsec);
  // logd ("tm.tv_sec: %ld  tm.tv_nsec: %ld", tm.tv_sec, tm.tv_nsec);

  //    nanosleep (& tm, NULL);

  unsigned long ms_end = ms_get() + ms;
  unsigned long ctr = 0;
  while (ms_get() < ms_end) {
    usleep(32000L);
    // tm.tv_nsec = 1000000L;
    // nanosleep (& tm, NULL);

    ctr++;

    if (ctr > 25) { // 100L) {
      ctr = 0L;
      //        printf (".\b");
    }
  }
  return (ms);

  //*/

  return (ms);
}

static void *busy_thread(void *arg) {
  logd("busy_thread start");

  while (1) {
    unsigned long ms = 1L;
    //      usleep (ms * 1000L);
    struct timespec tm;
    tm.tv_sec = 0;
    tm.tv_sec = ms / 1000L;
    tm.tv_nsec = (ms % 1000L) * 1000000L;
    // printf ("tm.tv_sec: %ld  tm.tv_nsec: %ld\n", tm.tv_sec, tm.tv_nsec);
    // logd ("tm.tv_sec: %ld  tm.tv_nsec: %ld", tm.tv_sec, tm.tv_nsec);
    //    nanosleep (& tm, NULL);

    int ctr = 0;
    int max_cnt = 32; // 512;   1024 too much on N9, 512 OK
    for (ctr = 0; ctr < max_cnt; ctr++) {
      ms = ms_get();
    }

    printf(".");
  }

  pthread_exit(NULL);
  loge("busy_thread exit 2");

  return (NULL); // Compiler friendly ; never reach
}

// REMOVED char user_dev [DEF_BUF] variable @ 10/09/2016
// REMOVED char * user_char_dev_get function @ 10/09/2016
// REMOVED char * upper_set function @ 10/09/2016
// REMOVED char * lower_set function @ 10/09/2016
// REMOVED int file_find  function @ 10/09/2016
// REMOVED int flags_file_get function @ 10/09/2016
// REMOVED int file_get function @ 10/09/2016
#define HD_MW 256
void hex_dump(char *prefix, int width, unsigned char *buf, int len) {
  if (0) //! strncmp (prefix, "AUDIO: ", strlen ("AUDIO: ")))
    len = len;
  else if (!ena_log_hexdu)
    return;
  // loge ("hex_dump prefix: \"%s\"  width: %d   buf: %p  len: %d", prefix,
  // width, buf, len);

  if (buf == NULL || len <= 0)
    return;

  if (len > max_hex_dump)
    len = max_hex_dump;

  char tmp[3 * HD_MW + 8] = ""; // Handle line widths up to HD_MW
  char line[3 * HD_MW + 8] = "";
  if (width > HD_MW)
    width = HD_MW;
  int i, n;
  line[0] = 0;

  if (prefix)
    // strlcpy (line, prefix, sizeof (line));
    strlcat(line, prefix, sizeof(line));

  snprintf(tmp, sizeof(tmp), " %8.8x ", 0);
  strlcat(line, tmp, sizeof(line));

  for (i = 0, n = 1; i < len;
       i++, n++) { // i keeps incrementing, n gets reset to 0 each line

    snprintf(tmp, sizeof(tmp), "%2.2x ", buf[i]);
    strlcat(line, tmp, sizeof(line)); // Append 2 bytes hex and space to line

    if (n == width) { // If at specified line width
      n = 0;          // Reset position in line counter
      logd(line);     // Log line

      line[0] = 0;
      if (prefix)
        // strlcpy (line, prefix, sizeof (line));
        strlcat(line, prefix, sizeof(line));

      // snprintf (tmp, sizeof (tmp), " %8.8x ", i + 1);
      snprintf(tmp, sizeof(tmp), "     %4.4x ", i + 1);
      strlcat(line, tmp, sizeof(line));
    } else if (i == len - 1) // Else if at last byte
      logd(line);            // Log line
  }
}

// REMOVED int util_insmod function @ 10/09/2016
// REMOVED  int file_write_many  function @ 10/09/2016
// REMOVED  int file_write  function @ 10/09/2016
// REMOVED  void * file_read function @ 10/09/2016
// REMOVED int insmod_internal function @ 10/09/2016

#define ERROR_CODE_NUM 56
static const char *error_code_str[ERROR_CODE_NUM + 1] = {
    "Success",
    "Unknown HCI Command",
    "Unknown Connection Identifier",
    "Hardware Failure",
    "Page Timeout",
    "Authentication Failure",
    "PIN or Key Missing",
    "Memory Capacity Exceeded",
    "Connection Timeout",
    "Connection Limit Exceeded",
    "Synchronous Connection to a Device Exceeded",
    "ACL Connection Already Exists",
    "Command Disallowed",
    "Connection Rejected due to Limited Resources",
    "Connection Rejected due to Security Reasons",
    "Connection Rejected due to Unacceptable BD_ADDR",
    "Connection Accept Timeout Exceeded",
    "Unsupported Feature or Parameter Value",
    "Invalid HCI Command Parameters",
    "Remote User Terminated Connection",
    "Remote Device Terminated Connection due to Low Resources",
    "Remote Device Terminated Connection due to Power Off",
    "Connection Terminated by Local Host",
    "Repeated Attempts",
    "Pairing Not Allowed",
    "Unknown LMP PDU",
    "Unsupported Remote Feature / Unsupported LMP Feature",
    "SCO Offset Rejected",
    "SCO Interval Rejected",
    "SCO Air Mode Rejected",
    "Invalid LMP Parameters",
    "Unspecified Error",
    "Unsupported LMP Parameter Value",
    "Role Change Not Allowed",
    "LMP Response Timeout",
    "LMP Error Transaction Collision",
    "LMP PDU Not Allowed",
    "Encryption Mode Not Acceptable",
    "Link Key Can Not be Changed",
    "Requested QoS Not Supported",
    "Instant Passed",
    "Pairing with Unit Key Not Supported",
    "Different Transaction Collision",
    "Reserved",
    "QoS Unacceptable Parameter",
    "QoS Rejected",
    "Channel Classification Not Supported",
    "Insufficient Security",
    "Parameter out of Mandatory Range",
    "Reserved",
    "Role Switch Pending",
    "Reserved",
    "Reserved Slot Violation",
    "Role Switch Failed",
    "Extended Inquiry Response Too Large",
    "Simple Pairing Not Supported by Host",
    "Host Busy - Pairing",
};

const char *hci_err_get(uint8_t status) {
  const char *str;
  if (status <= ERROR_CODE_NUM)
    str = error_code_str[status];
  else
    str = "Unknown HCI Error";
  return (str);
}

// For REUSEADDR only
//#define SK_NO_REUSE     0
//#define SK_CAN_REUSE    1

#ifndef SK_FORCE_REUSE
#define SK_FORCE_REUSE 2
#endif

#ifndef SO_REUSEPORT
#define SO_REUSEPORT 15
#endif

// ?? Blocked in Android ?          sock_tmo_set setsockopt SO_REUSEPORT errno:
// 92 (Protocol not available)
/* TODO:Uncomment for TCP support
int sock_reuse_set (int fd) {
    errno = 0;
    int val = SK_FORCE_REUSE;//SK_CAN_REUSE;
    int ret = setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, & val, sizeof (val));
    if (ret != 0)
        loge ("sock_tmo_set setsockopt SO_REUSEADDR errno: %d (%s)", errno,
strerror (errno));
    else
        logd ("sock_tmo_set setsockopt SO_REUSEADDR Success");
    return (0);
}

int sock_tmo_set (int fd, int tmo) {                                 // tmo =
timeout in milliseconds
    struct timeval tv = {0, 0};
    tv.tv_sec = tmo / 1000;                                               //
Timeout in seconds
    tv.tv_usec = (tmo % 1000) * 1000;
    errno = 0;
    int ret = setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) & tv,
sizeof (struct timeval));
    if (ret != 0)
        loge ("sock_tmo_set setsockopt SO_RCVTIMEO errno: %d (%s)", errno,
strerror (errno));
    else
        logv ("sock_tmo_set setsockopt SO_RCVTIMEO Success");
    //errno = 0;
    //ret = setsockopt (fd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *) & tv,
sizeof (struct timeval));
    //if (ret != 0) {
    //  loge ("timeout_set setsockopt SO_SNDTIMEO errno: %d (%s)", errno,
strerror (errno));
    //}
    return (0);
}

*/
// REMOVED int pid_get function @ 10/09/2016
// REMOVED int kill_gentel_first variable @ 10/09/2016
// REMOVED int pid_kill function @ 10/09/2016
// REMOVED int killall function @ 10/09/2016

// Buffers: Audio, Video, identical code, should generalize

#define aud_buf_BUFS_SIZE 65536 * 4 // Up to 256 Kbytes
int aud_buf_bufs_size = aud_buf_BUFS_SIZE;

#define NUM_aud_buf_BUFS                                                       \
  16 // Maximum of NUM_aud_buf_BUFS - 1 in progress; 1 is never used
int num_aud_buf_bufs = NUM_aud_buf_BUFS;

char aud_buf_bufs[NUM_aud_buf_BUFS][aud_buf_BUFS_SIZE];

int aud_buf_lens[NUM_aud_buf_BUFS] = {0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0};

int aud_buf_buf_tail = 0; // Tail is next index for writer to write to.   If
                          // head = tail, there is no info.
int aud_buf_buf_head = 0; // Head is next index for reader to read from.

int aud_buf_errs = 0;
int aud_max_bufs = 0;
int aud_sem_tail = 0;
int aud_sem_head = 0;

char *aud_write_tail_buf_get(int len) { // Get tail buffer to write to

  if (len > aud_buf_BUFS_SIZE) {
    loge("!!!!!!!!!! aud_write_tail_buf_get too big len: %d",
         len); // E/aud_write_tail_buf_get(10699): !!!!!!!!!!
               // aud_write_tail_buf_get too big len: 66338
    return (NULL);
  }

  int bufs = aud_buf_buf_tail - aud_buf_buf_head;
  if (bufs < 0)               // If underflowed...
    bufs += num_aud_buf_bufs; // Wrap
  // logd ("aud_write_tail_buf_get start bufs: %d  head: %d  tail: %d", bufs,
  // aud_buf_buf_head, aud_buf_buf_tail);

  if (bufs > aud_max_bufs) // If new maximum buffers in progress...
    aud_max_bufs = bufs;   // Save new max
  if (bufs >= num_aud_buf_bufs -
                  1) { // If room for another (max = NUM_aud_buf_BUFS - 1)
    loge("aud_write_tail_buf_get out of aud_buf_bufs");
    aud_buf_errs++;
    // aud_buf_buf_tail = aud_buf_buf_head = 0;                        // Drop
    // all buffers
    return (NULL);
  }

  int max_retries = 4;
  int retries = 0;
  for (retries = 0; retries < max_retries; retries++) {
    aud_sem_tail++;
    if (aud_sem_tail == 1)
      break;
    aud_sem_tail--;
    loge("aud_sem_tail wait");
    ms_sleep(10);
  }
  if (retries >= max_retries) {
    loge("aud_sem_tail could not be acquired");
    return (NULL);
  }

  if (aud_buf_buf_tail < 0 ||
      aud_buf_buf_tail > num_aud_buf_bufs - 1) // Protect
    aud_buf_buf_tail &= num_aud_buf_bufs - 1;

  aud_buf_buf_tail++;

  if (aud_buf_buf_tail < 0 || aud_buf_buf_tail > num_aud_buf_bufs - 1)
    aud_buf_buf_tail &= num_aud_buf_bufs - 1;

  char *ret = aud_buf_bufs[aud_buf_buf_tail];
  aud_buf_lens[aud_buf_buf_tail] = len;

  // logd ("aud_write_tail_buf_get done  ret: %p  bufs: %d  tail len: %d  head:
  // %d  tail: %d", ret, bufs, len, aud_buf_buf_head, aud_buf_buf_tail);

  aud_sem_tail--;

  return (ret);
}

char *aud_read_head_buf_get(int *len) { // Get head buffer to read from

  if (len == NULL) {
    loge("!!!!!!!!!! aud_read_head_buf_get");
    return (NULL);
  }
  *len = 0;

  int bufs = aud_buf_buf_tail - aud_buf_buf_head;
  if (bufs < 0)               // If underflowed...
    bufs += num_aud_buf_bufs; // Wrap
  // logd ("aud_read_head_buf_get start bufs: %d  head: %d  tail: %d", bufs,
  // aud_buf_buf_head, aud_buf_buf_tail);

  if (bufs <= 0) { // If no buffers are ready...
    if (ena_log_extra)
      logd("aud_read_head_buf_get no aud_buf_bufs");
    // aud_buf_errs ++;  // Not an error; just no data
    // aud_buf_buf_tail = aud_buf_buf_head = 0;                          // Drop
    // all buffers
    return (NULL);
  }

  int max_retries = 4;
  int retries = 0;
  for (retries = 0; retries < max_retries; retries++) {
    aud_sem_head++;
    if (aud_sem_head == 1)
      break;
    aud_sem_head--;
    loge("aud_sem_head wait");
    ms_sleep(10);
  }
  if (retries >= max_retries) {
    loge("aud_sem_head could not be acquired");
    return (NULL);
  }

  if (aud_buf_buf_head < 0 ||
      aud_buf_buf_head > num_aud_buf_bufs - 1) // Protect
    aud_buf_buf_head &= num_aud_buf_bufs - 1;

  aud_buf_buf_head++;

  if (aud_buf_buf_head < 0 || aud_buf_buf_head > num_aud_buf_bufs - 1)
    aud_buf_buf_head &= num_aud_buf_bufs - 1;

  char *ret = aud_buf_bufs[aud_buf_buf_head];
  *len = aud_buf_lens[aud_buf_buf_head];

  // logd ("aud_read_head_buf_get done  ret: %p  bufs: %d  head len: %d  head:
  // %d  tail: %d", ret, bufs, * len, aud_buf_buf_head, aud_buf_buf_tail);

  aud_sem_head--;

  return (ret);
}

#define vid_buf_BUFS_SIZE 65536 * 4 // Up to 256 Kbytes
int vid_buf_bufs_size = vid_buf_BUFS_SIZE;

#define NUM_vid_buf_BUFS                                                       \
  16 // Maximum of NUM_vid_buf_BUFS - 1 in progress; 1 is never used
int num_vid_buf_bufs = NUM_vid_buf_BUFS;

char vid_buf_bufs[NUM_vid_buf_BUFS][vid_buf_BUFS_SIZE];

int vid_buf_lens[NUM_vid_buf_BUFS] = {0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0};

int vid_buf_buf_tail = 0; // Tail is next index for writer to write to.   If
                          // head = tail, there is no info.
int vid_buf_buf_head = 0; // Head is next index for reader to read from.

int vid_buf_errs = 0;
int vid_max_bufs = 0;
int vid_sem_tail = 0;
int vid_sem_head = 0;

char *vid_write_tail_buf_get(int len) { // Get tail buffer to write to

  if (len > vid_buf_BUFS_SIZE) {
    loge("!!!!!!!!!! vid_write_tail_buf_get too big len: %d",
         len); // E/vid_write_tail_buf_get(10699): !!!!!!!!!!
               // vid_write_tail_buf_get too big len: 66338
    return (NULL);
  }

  int bufs = vid_buf_buf_tail - vid_buf_buf_head;
  if (bufs < 0)               // If underflowed...
    bufs += num_vid_buf_bufs; // Wrap
  // logd ("vid_write_tail_buf_get start bufs: %d  head: %d  tail: %d", bufs,
  // vid_buf_buf_head, vid_buf_buf_tail);

  if (bufs > vid_max_bufs) // If new maximum buffers in progress...
    vid_max_bufs = bufs;   // Save new max
  if (bufs >= num_vid_buf_bufs -
                  1) { // If room for another (max = NUM_vid_buf_BUFS - 1)
    loge("vid_write_tail_buf_get out of vid_buf_bufs");
    vid_buf_errs++;
    // vid_buf_buf_tail = vid_buf_buf_head = 0;                        // Drop
    // all buffers
    return (NULL);
  }

  int max_retries = 4;
  int retries = 0;
  for (retries = 0; retries < max_retries; retries++) {
    vid_sem_tail++;
    if (vid_sem_tail == 1)
      break;
    vid_sem_tail--;
    loge("vid_sem_tail wait");
    ms_sleep(10);
  }
  if (retries >= max_retries) {
    loge("vid_sem_tail could not be acquired");
    return (NULL);
  }

  if (vid_buf_buf_tail < 0 ||
      vid_buf_buf_tail > num_vid_buf_bufs - 1) // Protect
    vid_buf_buf_tail &= num_vid_buf_bufs - 1;

  vid_buf_buf_tail++;

  if (vid_buf_buf_tail < 0 || vid_buf_buf_tail > num_vid_buf_bufs - 1)
    vid_buf_buf_tail &= num_vid_buf_bufs - 1;

  char *ret = vid_buf_bufs[vid_buf_buf_tail];
  vid_buf_lens[vid_buf_buf_tail] = len;

  // logd ("vid_write_tail_buf_get done  ret: %p  bufs: %d  tail len: %d  head:
  // %d  tail: %d", ret, bufs, len, vid_buf_buf_head, vid_buf_buf_tail);

  vid_sem_tail--;

  return (ret);
}

char *vid_read_head_buf_get(int *len) { // Get head buffer to read from

  if (len == NULL) {
    loge("!!!!!!!!!! vid_read_head_buf_get");
    return (NULL);
  }
  *len = 0;

  int bufs = vid_buf_buf_tail - vid_buf_buf_head;
  if (bufs < 0)               // If underflowed...
    bufs += num_vid_buf_bufs; // Wrap
  // logd ("vid_read_head_buf_get start bufs: %d  head: %d  tail: %d", bufs,
  // vid_buf_buf_head, vid_buf_buf_tail);

  if (bufs <= 0) { // If no buffers are ready...
    if (ena_log_extra)
      logd("vid_read_head_buf_get no vid_buf_bufs");
    // vid_buf_errs ++;  // Not an error; just no data
    // vid_buf_buf_tail = vid_buf_buf_head = 0;                          // Drop
    // all buffers
    return (NULL);
  }

  int max_retries = 4;
  int retries = 0;
  for (retries = 0; retries < max_retries; retries++) {
    vid_sem_head++;
    if (vid_sem_head == 1)
      break;
    vid_sem_head--;
    loge("vid_sem_head wait");
    // ms_sleep(10);
  }
  if (retries >= max_retries) {
    loge("vid_sem_head could not be acquired");
    return (NULL);
  }

  if (vid_buf_buf_head < 0 ||
      vid_buf_buf_head > num_vid_buf_bufs - 1) // Protect
    vid_buf_buf_head &= num_vid_buf_bufs - 1;

  vid_buf_buf_head++;

  if (vid_buf_buf_head < 0 || vid_buf_buf_head > num_vid_buf_bufs - 1)
    vid_buf_buf_head &= num_vid_buf_bufs - 1;

  char *ret = vid_buf_bufs[vid_buf_buf_head];
  *len = vid_buf_lens[vid_buf_buf_head];

  // logd ("vid_read_head_buf_get done  ret: %p  bufs: %d  head len: %d  head:
  // %d  tail: %d", ret, bufs, * len, vid_buf_buf_head, vid_buf_buf_tail);

  vid_sem_head--;

  return (ret);
}

//#endif  //#ifndef UTILS_INCLUDED

// Client/Server:

//#ifdef  CS_AF_UNIX                                                      // For
// Address Family UNIX sockets
//#include <sys/un.h>
//#else                                                                   // For
// Address Family NETWORK sockets

// Unix datagrams requires other write permission for /dev/socket, or somewhere
// else (ext, not FAT) writable.

//#define CS_AF_UNIX        // Use network sockets to avoid filesystem
// permission issues w/ Unix Domain Address Family sockets
#define CS_DGRAM // Use datagrams, not streams/sessions

#ifdef CS_AF_UNIX // For Address Family UNIX sockets
//#include <sys/un.h>
#define DEF_API_SRVSOCK "/dev/socket/srv_spirit"
#define DEF_API_CLISOCK "/dev/socket/cli_spirit"
char api_srvsock[DEF_BUF] = DEF_API_SRVSOCK;
char api_clisock[DEF_BUF] = DEF_API_CLISOCK;
#define CS_FAM AF_UNIX

#else // For Address Family NETWORK sockets
//#include <netinet/in.h>
//#include <netdb.h>
#define CS_FAM AF_INET
#endif

#ifdef CS_DGRAM
#define CS_SOCK_TYPE SOCK_DGRAM
#else
#define CS_SOCK_TYPE SOCK_STREAM
#endif

#define RES_DATA_MAX 1280

#ifndef NDEBUG
char *usb_vid_get(int vid) {
  switch (vid) {
  case USB_VID_GOO:
    return ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Google");
  case USB_VID_HTC:
    return ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! HTC");
  case USB_VID_MOT:
    return ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Motorola");
  case USB_VID_SAM:
    return ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Samsung");
  case USB_VID_O1A:
    return ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Other/Samsung?"); // 0xfff6 /
                                                                // Samsung ?
  case USB_VID_SON:
    return ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Sony");
  case USB_VID_LGE:
    return ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! LGE");
  case USB_VID_LIN:
    return ("Linux");
  case USB_VID_QUA:
    return ("Qualcomm");
  case USB_VID_COM:
    return ("Comneon");
  case USB_VID_ASE:
    return ("Action Star");
  }
  return ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  Unknown VID"); //: %d", vid);
}
#endif
