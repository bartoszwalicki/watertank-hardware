#include "ssc-ultrasonic_distance.h"

uint16_t get_distance(uart_port_t uart_port_num) {
  char get_distance_command[] = {0x55};
  int length = 0;
  uint8_t data[2];
  uart_write_bytes(uart_port_num, get_distance_command,
                   sizeof(get_distance_command));

  uart_get_buffered_data_len(uart_port_num, (size_t *)&length);

  while (length < 2) {
    uart_get_buffered_data_len(uart_port_num, (size_t *)&length);
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }

  length = 0;

  uart_read_bytes(uart_port_num, data, 2, 20 / portTICK_PERIOD_MS);

  uint8_t high_byte = data[0];
  uint8_t low_byte = data[1];
  uint16_t meas = (high_byte << 8) | low_byte;

  return meas;
}

int8_t get_temperature(uart_port_t uart_port_num) {
  uint8_t data = 0;
  char get_temp_command[] = {0x50};

  uart_write_bytes(uart_port_num, get_temp_command, sizeof(get_temp_command));
  vTaskDelay(50 / portTICK_PERIOD_MS);
  uart_read_bytes(uart_port_num, &data, 1, 20 / portTICK_PERIOD_MS);

  int8_t result = data - 45;

  return result;
}

void read_distance_task(void *arg) {
  while (1) {
    ESP_LOGI(TAG_U, "Reading distance...");
    uint16_t meas1 = get_distance(UART_NUM_2);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    uint16_t meas2 = get_distance(UART_NUM_1);
    struct TankData tank_data;

    ESP_LOGI(TAG_U, "Reading distance finish...");
    tank_data.tank_id = 0;
    tank_data.waterlevel = meas2;
    xQueueSend(tank_level_handle_queue, &tank_data, portMAX_DELAY);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    tank_data.tank_id = 1;
    tank_data.waterlevel = meas1;
    xQueueSend(tank_level_handle_queue, &tank_data, portMAX_DELAY);

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void read_temperature_task(void *arg) {
  while (1) {
    int8_t temp1 = get_temperature(UART_NUM_2);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    int8_t temp2 = get_temperature(UART_NUM_1);
    struct TankData tank_data;

    tank_data.tank_id = 0;
    tank_data.temperature = temp2;
    xQueueSend(tank_temperature_handle_queue, &tank_data, portMAX_DELAY);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    tank_data.tank_id = 1;
    tank_data.temperature = temp1;
    xQueueSend(tank_temperature_handle_queue, &tank_data, portMAX_DELAY);

    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

void init_uart() {
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_APB,
  };

  uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART_NUM_2, &uart_config);
  uart_set_pin(UART_NUM_2, GPIO_NUM_23, GPIO_NUM_22, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);

  uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART_NUM_1, &uart_config);
  uart_set_pin(UART_NUM_1, GPIO_NUM_19, GPIO_NUM_18, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);

  ESP_LOGI(TAG_U,"Uart sensor init");
}