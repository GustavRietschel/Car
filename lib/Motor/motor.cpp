#include "motor.h"

Motor::Motor(mcpwm_unit_t unit, mcpwm_timer_t timer, mcpwm_io_signals_t io_signal, int pin) 
    : mcpwm_unit(unit), mcpwm_timer(timer), mcpwm_io_signal(io_signal), gpio_pin(pin) {}

bool Motor::init()
{
    mcpwm_config_t pwm_config = {
        .frequency = MOTOR_PWM_FREQUENCY,
        .cmpr_a = 0.0,
        .cmpr_b = 0.0,
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER,
    };


    esp_err_t err1 = mcpwm_gpio_init(mcpwm_unit, mcpwm_io_signal, gpio_pin);
    esp_err_t err2 = mcpwm_init(mcpwm_unit, mcpwm_timer, &pwm_config);
    
    if (err1 != ESP_OK || err2 != ESP_OK) {
        return false;
    }
    

    mcpwm_start(mcpwm_unit, mcpwm_timer);
    return true;
}

void Motor::setSpeed(int xValue)
{
    float duty_cycle = calculateSpeed(xValue);

    if (duty_cycle < 0) duty_cycle = 0;
    if (duty_cycle > 100) duty_cycle = 100;
        
    mcpwm_set_duty(mcpwm_unit, mcpwm_timer, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(mcpwm_unit, mcpwm_timer, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

void Motor::stop()
{
    mcpwm_set_duty(mcpwm_unit, mcpwm_timer, MCPWM_OPR_A, 0);
    mcpwm_stop(mcpwm_unit, mcpwm_timer);
}

float Motor::calculateSpeed(int xValue)
{
    if (xValue >= 2900)
    {
        printf("%d \n", xValue);
        printf("%f \n", 100.0 + ((xValue - 2900) * (0.0 - 100.0)) / (4095 - 2900));
        return 100.0 + (xValue - 2900) * (0.0 - 100.0) / 4095 - 2900;
    } else 
    {
        printf("100.0 \n");
        return 100.0;
    }
    
}