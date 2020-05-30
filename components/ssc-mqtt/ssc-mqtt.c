#include "ssc-mqtt.h"

void mqtt_publish_temperature(struct TankData *tank_data) {
  char temp_value[50];
  sprintf(temp_value, "waterlevel,tank=%d temperature=%d %" PRId64 "\n", tank_data->tank_id , tank_data->temperature, tank_data->timestamp);

  esp_mqtt_client_publish(_client, "waterlevel/", temp_value, 0, 0, 0);
}

void mqtt_publish_distance(struct TankData *tank_data) {
  char temp_value[50];
  sprintf(temp_value, "waterlevel,tank=%d distance=%d %" PRId64 "\n" , tank_data->tank_id , tank_data->waterlevel, tank_data->timestamp);

  esp_mqtt_client_publish(_client, "waterlevel/", temp_value, 0, 0, 0);
}

void observe_temperature_queue() {
    struct TankData tank_data;
    while(1) {
        if(xQueueReceive(tank_temperature_handle_queue, &tank_data, portMAX_DELAY)) {
            ESP_LOGI(TAG,"Got temperature from queue tank_id: %i, value: %i", tank_data.tank_id, tank_data.temperature);
            mqtt_publish_temperature(&tank_data);
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void observe_distance_queue() {
    struct TankData tank_data;
    while(1) {
        if(xQueueReceive(tank_level_handle_queue, &tank_data, portMAX_DELAY)) {
            ESP_LOGI(TAG,"Got distance from queue tank_id: %i, value: %i", tank_data.tank_id, tank_data.waterlevel);
            mqtt_publish_distance(&tank_data);
        }

        vTaskDelay(1500 / portTICK_RATE_MS);
    }
}