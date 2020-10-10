#include "ssc-ultrasonic_distance.h"

void swap(uint16_t *p, uint16_t *q) {
  uint16_t t;

  t = *p;
  *p = *q;
  *q = t;
}

void sort(uint16_t a[], uint8_t n) {
  uint8_t i, j;

  for (i = 0; i < n - 1; i++) {
    for (j = 0; j < n - i - 1; j++) {
      if (a[j] > a[j + 1])
        swap(&a[j], &a[j + 1]);
    }
  }
}

uint16_t get_distance(uart_port_t uart_port_num) {
  uint16_t meas;
  uint8_t repeat_measurment_count = 100;
  uint16_t meas_arr[repeat_measurment_count];

  for (uint8_t i = 0; i < repeat_measurment_count; i++) {
    char get_distance_command[] = {0x55};
    int length = 0;
    uint8_t data[4];
    uart_write_bytes(uart_port_num, get_distance_command,
                     sizeof(get_distance_command));

    vTaskDelay(65 / portTICK_PERIOD_MS);
    uart_get_buffered_data_len(uart_port_num, (size_t *)&length);

    length = 0;

    uart_read_bytes(uart_port_num, data, 4, 20 / portTICK_PERIOD_MS);

    uint8_t high_byte = data[1];
    uint8_t low_byte = data[2];
    uint16_t temp_meas = (high_byte << 8) | low_byte;

    uint8_t checksum = (data[0] + data[1] + data[2]) & 0x00ff;
    meas_arr[i] = temp_meas;

    // printf("0x%02X, 0x%02X, 0x%02X, 0x%02X | 0x%02X || %5u \n\r",
    // data[0],data[1],data[2], data[3],checksum, temp_meas );
  }

  sort(&meas_arr,repeat_measurment_count);
  printf("%i || %5u \n\r", uart_port_num ,meas_arr[repeat_measurment_count - 1]);

  return meas_arr[repeat_measurment_count - 1];
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
  uart_set_pin(UART_NUM_2, GPIO_NUM_22, GPIO_NUM_23, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);

  uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART_NUM_1, &uart_config);
  uart_set_pin(UART_NUM_1, GPIO_NUM_21, GPIO_NUM_19, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);

  ESP_LOGI(TAG_U, "Uart sensor init");
}