#ifndef SERVO_H
#define SERVO_H

#include "driver/ledc.h"

class Servo
{
public:
    Servo(int pin, int minUs = 1000, int maxUs = 2000, uint32_t freqHz = 50);
    void init();
    void write(int angle);

private:
    int pin;
    int minUs;
    int maxUs;
    uint32_t freqHz;
    ledc_channel_t channel;
    ledc_timer_t timer;

    int calculateAngle(int yValue);
};

#endif