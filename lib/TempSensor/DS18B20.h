#ifndef DS18B20_H
#define DS18B20_H

#include "driver/gpio.h"
#include <vector>

class DS18B20 {
public:
    DS18B20(gpio_num_t gpio_pin);
    bool init();
    float readTemperature();

private:
    gpio_num_t pin;

    void writeBit(bool bit);
    bool readBit();
    void writeByte(uint8_t data);
    uint8_t readByte();
    bool resetPulse();
};

#endif // DS18B20_H
