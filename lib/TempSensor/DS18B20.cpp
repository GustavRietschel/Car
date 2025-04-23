#include "DS18B20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

DS18B20::DS18B20(gpio_num_t gpio_pin) : pin(gpio_pin) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 1);
}

bool DS18B20::init() {
    return resetPulse();
}

bool DS18B20::resetPulse() {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(480); 
    gpio_set_level(pin, 1);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    ets_delay_us(70); 

    bool presence = gpio_get_level(pin) == 0;
    ets_delay_us(410); 
    return presence;
}

void DS18B20::writeBit(bool bit) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(bit ? 10 : 60); 
    gpio_set_level(pin, 1);
    ets_delay_us(bit ? 55 : 5); 
}

bool DS18B20::readBit() {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(2);
    gpio_set_level(pin, 1);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    ets_delay_us(10); 
    bool bit = gpio_get_level(pin);
    ets_delay_us(50); 
    return bit;
}

void DS18B20::writeByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        writeBit(data & 0x01);
        data >>= 1;
    }
}

uint8_t DS18B20::readByte() {
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        if (readBit()) {
            data |= (1 << i);
        }
    }
    return data;
}

float DS18B20::readTemperature() {
    if (!resetPulse()) {
        return -127.0; 
    }

    
    writeByte(0xCC); 
    writeByte(0x44); 

    
    vTaskDelay(pdMS_TO_TICKS(750));

    if (!resetPulse()) {
        return -127.0; 
    }

    writeByte(0xCC);
    writeByte(0xBE); 

    
    uint8_t lsb = readByte();
    uint8_t msb = readByte();

    int16_t temp = (msb << 8) | lsb;

   
    return temp / 16.0;
}
