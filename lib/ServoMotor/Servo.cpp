#include "Servo.h"

Servo::Servo(int pin, int minUs, int maxUs, uint32_t freqHz)
    : pin(pin), minUs(minUs), maxUs(maxUs), freqHz(freqHz),
      channel(LEDC_CHANNEL_0), timer(LEDC_TIMER_0)
{
}

void Servo::init()
{
    ledc_timer_config_t timerCfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .timer_num = timer,
        .freq_hz = freqHz,
    };

    ledc_timer_config(&timerCfg);

    ledc_channel_config_t channelCfg = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer,
        .duty = 0,
        .hpoint = 0,
    };

    ledc_channel_config(&channelCfg);

    printf("Init erfolgreich");
}

void Servo::write(int yValue)
{
    printf("Servo Write gestartet (YValue: %d)...\n", yValue);
    int angle = calculateAngle(yValue);
    printf("Angle: %d", angle);
    if (angle < 0)
        angle = 0;
    if (angle > 180)
        angle = 180;

    int pulseWidth = minUs + (angle * (maxUs - minUs) / 100);
    printf("Pulse Width: %d\n", pulseWidth);
    int duty = (pulseWidth * ((1 << 16) - 1)) / (1000000 / freqHz);
    printf("Duty Cycle: %d\n", duty);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);

    printf("Servo Write abgeschlossen!\n");
}

// Use real values when testet with real car
int Servo::calculateAngle(int yValue)
{
    int angle = 0;
    // Steer right 
    if (yValue >= 3100) {
        angle = 180;
    } else if (yValue <= 2800) { // Steer left
        angle = 0;
    } else {
        angle = 90; // Straight
    }

    return angle;
}


