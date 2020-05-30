# Water tank level monitor

This project is built with ESP32 and esp-idf framework. It periodically sends data to MQTT broker using TLS connection. It is collecting data from two UART ultrasonic sensors placed inside two separate large rain water tanks.

## Project structure

Project is divided on components: 

* `ssc-time` - timekeeping and synchronisation, generating timestamps
* `ssc-mqtt` - managing secure MQTT and sendind data to the broker
* `ssc-ultrasonic_distance` - communicating with UART sensors

## Secrets

To run project you need `client.crt`, `client.key` and `types.h` file. 

`types.h` contains `#define MQTT_ENDPOINT "mqtts://broker-addres.com:8883"`