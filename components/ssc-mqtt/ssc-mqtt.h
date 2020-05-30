#ifndef COMPONENTS_SSC_MQTT_SSC_MQTT_H_
#define COMPONENTS_SSC_MQTT_SSC_MQTT_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "../../main/types.h"
#include "../../main/secret.h"

extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");

extern QueueHandle_t tank_temperature_handle_queue;
extern QueueHandle_t tank_level_handle_queue;

esp_mqtt_client_handle_t _client;

static const char *TAG = "MQTTS";

void observe_temperature_queue(void);
void observe_distance_queue(void);

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {
	// your_context_t *context = event->context;
	switch (event->event_id) {
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
	}
	return ESP_OK;
}

static void mqtt_app_start(void) {
	const esp_mqtt_client_config_t mqtt_cfg = { .uri =
			MQTT_ENDPOINT, .event_handle =
			mqtt_event_handler, .client_cert_pem =
			(const char*) client_cert_pem_start, .client_key_pem =
			(const char*) client_key_pem_start, };

	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	_client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(_client);

	// xTaskCreate(observe_temperature_queue, "observe_temp_task", 2048, NULL, 10, NULL);
	// xTaskCreate(observe_distance_queue, "observe_distance_task", 2048, NULL, 10, NULL);
}

void mqtt_publish_temperature(struct TankData *tank_data);
void mqtt_publish_distance(struct TankData *tank_data);



#endif /* COMPONENTS_SSC_MQTT_SSC_MQTT_H_ */
