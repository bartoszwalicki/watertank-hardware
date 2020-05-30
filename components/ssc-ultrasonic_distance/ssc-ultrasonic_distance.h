#ifndef COMPONENTS_SSC_ULTRASONIC_DISTANCE_H_
#define COMPONENTS_SSC_ULTRASONIC_DISTANCE_H_

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

#include "../../main/types.h"

#define BUF_SIZE (1024)

static const char *TAG_U = "ULTRA";

extern QueueHandle_t tank_temperature_handle_queue;
extern QueueHandle_t tank_level_handle_queue;

uint16_t get_distance(uart_port_t uart_port_num);
int8_t get_temperature(uart_port_t uart_port_num);
void read_distance_task(void *arg);
void read_temperature_task(void *arg);
void init_uart();

#endif