#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"
#include "esp_log.h"

void obtain_time(void);
void time_sync_notification_cb(struct timeval *tv);
void sntp_sync_time(struct timeval *tv);
void show_current_time(void);
int64_t get_nanosecond_current_time(void);
int64_t get_milisecond_current_time(void);