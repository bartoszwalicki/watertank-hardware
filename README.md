# Water tank level monitor

This project is built with ESP32 and esp-idf framework. It periodically sends data to MQTT broker using TLS connection or HTTPS InfluxDB 2.0 Cloud endpoint. It is collecting data from two UART ultrasonic sensors placed inside two separate large rain water tanks.

## Project structure

Project is divided on components: 

* `ssc-time` - timekeeping and synchronisation, generating timestamps
* `ssc-mqtt` - managing secure MQTT and sendind data to the broker
* `ssc-ultrasonic_distance` - communicating with UART sensors
* `ssc-http` - HTTPS connection to InfluxDB 2.0 Cloud

## Secrets

To run project you need `client.crt`, `client.key` and `secret.h` file. 

To create `secret.h` lookup `secretEXAMPLE.h`.