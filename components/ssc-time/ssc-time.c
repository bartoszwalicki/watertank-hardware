#include "ssc-time.h"

static const char *TAG = "TIME";

void initialize_sntp(void) {
  ESP_LOGI(TAG, "Initializing SNTP");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_set_time_sync_notification_cb(time_sync_notification_cb);
  sntp_init();
}

void time_sync_notification_cb(struct timeval *tv) {
  ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void obtain_time(void) {
  initialize_sntp();

  time_t now = 0;
  struct tm timeinfo = {0};
  int retry = 0;
  const int retry_count = 10;
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET &&
         ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry,
             retry_count);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  time(&now);
  localtime_r(&now, &timeinfo);
}

void sntp_sync_time(struct timeval *tv) {
  settimeofday(tv, NULL);
  ESP_LOGI(TAG, "Time is synchronized from custom code");
  sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
}

void show_current_time() {
  char strftime_buf[64];
  time_t now;
  struct tm timeinfo;
  time(&now);

  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI(TAG, "The current date/time UTC is: %s", strftime_buf);
}

int64_t get_nanosecond_current_time() {
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  int64_t time_us = (int64_t)tv_now.tv_sec * 1000000000 + (int64_t)tv_now.tv_usec * 1000;

  return time_us;
}

int64_t get_milisecond_current_time() {
  struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t timestamp = (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));

  return timestamp;
}