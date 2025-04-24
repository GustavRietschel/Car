#include "motor.h"
#include "freertos/queue.h"

int pulse_count = 0;
uint64_t last_time = 0;

void IRAM_ATTR gpio_isr_handler(void* arg) { pulse_count++; }

Motor::Motor(mcpwm_unit_t unit, mcpwm_timer_t timer,
             mcpwm_io_signals_t io_signal, int pin, gpio_num_t encoderPin)
    : mcpwm_unit(unit), mcpwm_timer(timer), mcpwm_io_signal(io_signal),
      gpio_pin(pin), encoderPin(encoderPin)
{
}

bool Motor::init()
{
    // Configure pin and enable interrupts 
    gpio_config_t signal_io_cfg = {.pin_bit_mask = (1ULL << encoderPin),
                                   .mode = GPIO_MODE_INPUT,
                                   .pull_up_en = GPIO_PULLUP_ENABLE,
                                   .pull_down_en = GPIO_PULLDOWN_ENABLE,
                                   .intr_type = GPIO_INTR_POSEDGE};

    gpio_config(&signal_io_cfg);

    // Configure interrupts
    gpio_install_isr_service(0);
    gpio_isr_handler_add(encoderPin, gpio_isr_handler, (void*)encoderPin);

    // PWM Configuration 
    mcpwm_config_t pwm_cfg = {
        .frequency = MOTOR_PWM_FREQUENCY,
        .cmpr_a = 0.0,
        .cmpr_b = 0.0,
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER,
    };

    // Initialize the MCPWM GPIO pin
    esp_err_t err1 = mcpwm_gpio_init(mcpwm_unit, mcpwm_io_signal, gpio_pin);
    // Initialize the MCPWM timer with the configuration
    esp_err_t err2 = mcpwm_init(mcpwm_unit, mcpwm_timer, &pwm_cfg);

    if (err1 != ESP_OK || err2 != ESP_OK)
    {
        return false;
    }

    mcpwm_start(mcpwm_unit, mcpwm_timer);
    return true;
}

void Motor::setSpeed(int xValue)
{
    // xValue into duty_cycle
    float duty_cycle = calculateDutyCycle(xValue);

    if (duty_cycle < 0)
        duty_cycle = 0;
    if (duty_cycle > 100)
        duty_cycle = 100;

    // apply the duty_cycle 
    mcpwm_set_duty(mcpwm_unit, mcpwm_timer, MCPWM_OPR_A, duty_cycle);
    // apply duty type 
    mcpwm_set_duty_type(mcpwm_unit, mcpwm_timer, MCPWM_OPR_A,
                        MCPWM_DUTY_MODE_0);
}

void Motor::stop()
{
    // apply the duty_cycle 0
    mcpwm_set_duty(mcpwm_unit, mcpwm_timer, MCPWM_OPR_A, 0);
    // stop the motor
    mcpwm_stop(mcpwm_unit, mcpwm_timer);
}

float Motor::calculateDutyCycle(int xValue)
{
    // Check if Joystick is forward
    if (xValue >= 3000)
    {
        // xValue into %
        return 100.0 + (xValue - 2900) * (0.0 - 100.0) / 4095 - 2900;
    }
    else
    {
        return 100.0;
    }
}

float Motor::calculateSpeed()
{
    // current time in microseconds
    uint64_t current_time = esp_timer_get_time();

    // time difference since last calculation
    uint64_t time_diff = current_time - last_time;

    float speed_kmh = 0.0;

    // Only if time passed 
    if (time_diff > 0)
    {
        // calculate rpm 
        float rpm = (pulse_count * 60.0 * 1000000.0) / (time_diff * 270);

        // diameter in meter
        const float wheel_diameter = 0.04;
        const float wheel_circumference = 3.1416 * wheel_diameter;

        // rpm in kmh
        speed_kmh = (rpm * wheel_circumference * 60) / 1000;
    }
    pulse_count = 0;
    last_time = current_time;

    return speed_kmh;
}