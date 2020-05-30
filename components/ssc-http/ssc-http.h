#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"

#include "../../main/types.h"
#include "../../main/secret.h"

static const char *TAG_HTTP = "HTTP";

esp_err_t _http_event_handler(esp_http_client_event_t *evt);
void send_data_to_influxdb_cloud(struct TankData *tank_data);
