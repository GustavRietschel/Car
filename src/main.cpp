#include "connections.h"
#include "motor.h"
#include "DS18B20.h"

#define MotorPIN 18

// Data Struct for receiving Data 
typedef struct joystickData 
{
    int xValue;
    int yValue;
} joystickData;

// Data Struct for sending Data 
typedef struct carData 
{
    float temp;
    float speed;
} carData;

joystickData receivedData;

// Mac Address from Controller ESP32 
uint8_t macAddr[6] = {0x14, 0x2b, 0x2f, 0xc4, 0xc9, 0x4c};

void onDataSend(const uint8_t *macAddr, esp_now_send_status_t status) {
    if (status != 0) {
        printf("Error while sending!");
    }
}


void onDataReceived(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len) {
    memcpy(&receivedData, data, sizeof(receivedData));
}




extern "C" void app_main(void) 
{

    vTaskDelay(pdMS_TO_TICKS(3000));

    Motor motor(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, MotorPIN, GPIO_NUM_21);
    DS18B20 ds18b20(GPIO_NUM_4);

    if(!motor.init()) 
    {
        return;
    }

   ds18b20.init();

    ESP_NOW esp_now;

    // Creating Dummy Data to send to the controller 
    carData carData;

    carData.speed = 0;
    carData.temp = 20;


    // ESP_Now 
    esp_now.start(onDataSend, onDataReceived);
    esp_now.addNewESP(macAddr);

    while(1) 
    {
        
        motor.setSpeed(receivedData.xValue);

        carData.speed = motor.calculateSpeed();
        carData.temp = ds18b20.readTemperature();
        
        printf("%.2f\n", carData.temp);

        esp_now.sendData(macAddr, &carData, sizeof(carData));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

