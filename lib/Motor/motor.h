#ifndef MOTOR_H
#define MOTOR_h

#include "driver/mcpwm.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"

#define MOTOR_PWM_FREQUENCY   1000    
#define MOTOR_PWM_GPIO_PIN    19  


class Motor
{
    public: 
        Motor(mcpwm_unit_t unit, mcpwm_timer_t timer, mcpwm_io_signals_t io_signal, int pin, gpio_num_t encoderPin);
        bool init();
        void setSpeed(int xValue);
        void stop();
        float calculateSpeed();

    private: 
        mcpwm_unit_t mcpwm_unit;
        mcpwm_timer_t mcpwm_timer;
        mcpwm_io_signals_t mcpwm_io_signal;
        int gpio_pin;
        gpio_num_t encoderPin;
        int pulseCount;
        float calculateDutyCycle(int xValue);
        
};

#endif