#include "DS18B20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

DS18B20::DS18B20(gpio_num_t gpio_pin) : pin(gpio_pin)
{
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 1);
}

bool DS18B20::init() { return resetPulse(); }

bool DS18B20::resetPulse()
{
    // Reset Pin: output->low->high->input
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(480);
    gpio_set_level(pin, 1);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    ets_delay_us(70);

    // detect presence if low -> sensor detected
    bool presence = gpio_get_level(pin) == 0;
    ets_delay_us(410);
    return presence;
}

void DS18B20::writeBit(bool bit)
{
    // Output and Low
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    // if bit High wait 10µs if low wait 60µs
    ets_delay_us(bit ? 10 : 60);
    gpio_set_level(pin, 1);
    // if bit High wait 55µs if low wait 5µs
    ets_delay_us(bit ? 55 : 5);
}

bool DS18B20::readBit()
{
    // Start a read time slot by pulling the bus low
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);

    // short low pulse to initiate read slot
    ets_delay_us(2);

    // Release the bus by setting it high
    gpio_set_level(pin, 1);

    // input to read sensor response
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    // DS18B20 will pull the line low for a 0 or leave it high for a 1
    ets_delay_us(10);
    bool bit = gpio_get_level(pin);
    ets_delay_us(50);

    return bit;
}

void DS18B20::writeByte(uint8_t data)
{
    // split the byte into bits
    for (int i = 0; i < 8; i++)
    {
        // send the smallest bit (LSB)
        writeBit(data & 0x01);
        // Shift all bits to the rights so the next bit is the LSB
        data >>= 1;
    }
}

uint8_t DS18B20::readByte()
{
    uint8_t data = 0;

    // split byte into bits
    for (int i = 0; i < 8; i++)
    {
        // if the current bit is 1
        if (readBit())
        {
            // Set the corresponding bit in our result byte
            // (1 << i) shifts a 1 to position i, then OR operation sets that
            // bit
            data |= (1 << i);
        }
    }
    return data;
}

float DS18B20::readTemperature()
{
    if (!resetPulse())
    {
        return -127.0;
    }

    // Skip ROM
    writeByte(0xCC);
    // start temperatur conversion
    writeByte(0x44);

    vTaskDelay(pdMS_TO_TICKS(750));

    if (!resetPulse())
    {
        return -127.0;
    }

    // Skip ROM
    writeByte(0xCC);
    // Read Scratchpad
    writeByte(0xBE);

    // First to bytes from scratchpad
    uint8_t lsb = readByte();
    uint8_t msb = readByte();

    // combine lsb and msb into one 16 bit value
    // shift msb 8 bits to the left combine with lsb via OR
    int16_t temp = (msb << 8) | lsb;

    // DS18B20 has a resolution of 1/16 degree per bit
    return temp / 16.0;
}
