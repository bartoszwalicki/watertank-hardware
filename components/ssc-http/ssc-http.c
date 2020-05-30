#include "ssc-http.h"

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
  static char *output_buffer;

  static int output_len;
  switch (evt->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGD(TAG_HTTP, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGD(TAG_HTTP, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGD(TAG_HTTP, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(TAG_HTTP, "HTTP_EVENT_ON_HEADER, key=%s, value=%s",
             evt->header_key, evt->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(TAG_HTTP, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

    if (!esp_http_client_is_chunked_response(evt->client)) {
      if (evt->user_data) {
        memcpy(evt->user_data + output_len, evt->data, evt->data_len);
      } else {
        if (output_buffer == NULL) {
          output_buffer =
              (char *)malloc(esp_http_client_get_content_length(evt->client));
          output_len = 0;
          if (output_buffer == NULL) {
            ESP_LOGE(TAG_HTTP, "Failed to allocate memory for output buffer");
            return ESP_FAIL;
          }
        }
        memcpy(output_buffer + output_len, evt->data, evt->data_len);
      }
      output_len += evt->data_len;
    }

    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG_HTTP, "HTTP_EVENT_ON_FINISH");
    if (output_buffer != NULL) {
      // Response is accumulated in output_buffer. Uncomment the below line to
      // print the accumulated response ESP_LOG_BUFFER_HEX(TAG_HTTP,
      // output_buffer, output_len);
      free(output_buffer);
      output_buffer = NULL;
      output_len = 0;
    }
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG_HTTP, "HTTP_EVENT_DISCONNECTED");
    int mbedtls_err = 0;
    esp_err_t err =
        esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
    if (err != 0) {
      if (output_buffer != NULL) {
        free(output_buffer);
        output_buffer = NULL;
        output_len = 0;
      }
      ESP_LOGI(TAG_HTTP, "Last esp error code: 0x%x", err);
      ESP_LOGI(TAG_HTTP, "Last mbedtls failure: 0x%x", mbedtls_err);
    }
    break;
  }
  return ESP_OK;
};

void send_data_to_influxdb_cloud(struct TankData *tank_data) {
  char temp_value[50];
  sprintf(temp_value,
          "waterlevel,tank=%d t=%d,w=%d %" PRId64 "\n",
          tank_data->tank_id, tank_data->temperature, tank_data->waterlevel,
          tank_data->timestamp);

  esp_http_client_config_t config = {.url = INFLUXDB2_CLOUD_URL,
                                     .event_handler = _http_event_handler};
  esp_http_client_handle_t client = esp_http_client_init(&config);

  const char *post_data = &temp_value;

  // POST
  // const char *post_data =
  //     "waterlevel,tank=0 distance=11111 1590698779274302000";
  esp_http_client_set_method(client, HTTP_METHOD_POST);
  esp_http_client_set_header(client, "Content-Type", "text/plain");
  esp_http_client_set_header(client, "Authorization", INFLUXDB2_TOKEN);
  esp_http_client_set_post_field(client, post_data, strlen(post_data));
  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK) {
    ESP_LOGI(TAG_HTTP, "HTTP POST Status = %d, content_length = %d",
             esp_http_client_get_status_code(client),
             esp_http_client_get_content_length(client));
  } else {
    ESP_LOGE(TAG_HTTP, "HTTP POST request failed: %s", esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);
};