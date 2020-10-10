#include "esp_event.h"
#include "esp_netif.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "ssc-http.h"
#include "ssc-time.h"
#include "ssc-ultrasonic_distance.h"
#include "types.h"

QueueHandle_t tank_temperature_handle_queue = NULL;
QueueHandle_t tank_level_handle_queue = NULL;

#define ULTRA_SENSOR_0 UART_NUM_1
#define ULTRA_SENSOR_1 UART_NUM_2

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

static const char *TAG_M = "MAIN";

void app_main(void) {
  ESP_LOGI(TAG_M, "[APP] Startup..");
  ESP_LOGI(TAG_M, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
  ESP_LOGI(TAG_M, "[APP] IDF version: %s", esp_get_idf_version());

  esp_log_level_set("*", ESP_LOG_VERBOSE);

  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  vTaskDelay(20 / portTICK_RATE_MS);
  ESP_ERROR_CHECK(example_connect());

  tank_temperature_handle_queue = xQueueCreate(5, sizeof(struct TankData));
  tank_level_handle_queue = xQueueCreate(5, sizeof(struct TankData));

  vTaskDelay(20 / portTICK_RATE_MS);
  init_uart();

  obtain_time();
  show_current_time();

  const int wakeup_time_sec = 30;
  ESP_LOGI(TAG_M, "Enabling timer wakeup, %ds", wakeup_time_sec);
  esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);

  struct TankData tankData;
  uint16_t distance_sensor_0 = 0;
  uint16_t distance_sensor_1 = 0;
  int64_t nanosecond_time = 0;

  while (1) {
    nanosecond_time = get_nanosecond_current_time();
    tankData.timestamp = nanosecond_time;

    distance_sensor_0 = get_distance(ULTRA_SENSOR_0);

    distance_sensor_1 = get_distance(ULTRA_SENSOR_1);

    printf("dist0: %5u, dist1: %5u time: %" PRId64 "\n\r", distance_sensor_0,
           distance_sensor_1, nanosecond_time);

    // Sensor 0
    tankData.tank_id = 0;
    tankData.waterlevel = distance_sensor_0;
    send_data_to_influxdb_cloud(&tankData);

    // Sensor 1
    tankData.tank_id = 1;
    tankData.waterlevel = distance_sensor_1;
    send_data_to_influxdb_cloud(&tankData);

    esp_deep_sleep_start();
    // vTaskDelay(9200 / portTICK_RATE_MS);
  }
}
