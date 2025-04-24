#include "DS18B20.h"
#include "Servo.h"
#include "connections.h"
#include "motor.h"

#include "string.h"

#define MotorPIN 18

// Data Struct for receiving Data
typedef struct joystickData
{
    int xValue;
    int yValue;
} joystickData;

joystickData receivedData;

// Data Struct for sending Data
typedef struct carData
{
    float temp;
    float speed;
} carData;

// Mac Address from Controller ESP32
uint8_t macAddr[6] = {0x14, 0x2b, 0x2f, 0xc4, 0xc9, 0x4c};

// Callback functions for ESP NOW
void onDataSend(const uint8_t* macAddr, esp_now_send_status_t status)
{
    if (status != 0)
    {
        printf("Error while sending!");
    }
}

void onDataReceived(const esp_now_recv_info_t* esp_now_info,
                    const uint8_t* data, int len)
{
    // Copy data into received data
    memcpy(&receivedData, data, sizeof(receivedData));
}

extern "C" void app_main(void)
{
    // Wait 2sec to get clear serial output
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Create all ojects needed for motor controll, steering, esp now and temperature
    Motor motor(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, MotorPIN, GPIO_NUM_21);
    DS18B20 ds18b20(GPIO_NUM_4);
    ESP_NOW esp_now;
    Servo servo(25);

    // Data object 
    carData carData;

    // Init all pheripherals  
    motor.init();
    ds18b20.init();
    servo.init();

    // ESP NOW Init
    esp_now.start(onDataSend, onDataReceived);
    esp_now.addNewESP(macAddr);

    // Do all the tasks every 100ms
    while (1)
    {
        motor.setSpeed(receivedData.xValue);
        // servo.write(receivedData.yValue); -> Leads to Crash need to be fixed

        // Set values before sending to controller
        carData.speed = motor.calculateSpeed();
        carData.temp = ds18b20.readTemperature();

        // Sending all data to the controller
        esp_now.sendData(macAddr, &carData, sizeof(carData));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
