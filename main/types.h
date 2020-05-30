#ifndef TYPES_H
#define TYPES_H

struct TankData {
    uint8_t tank_id;
    int8_t temperature;
    uint16_t waterlevel;
    int64_t timestamp;
};

#endif