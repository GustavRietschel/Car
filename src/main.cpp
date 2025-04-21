#include "connections.h"
#include <iostream>
#include "motor.h"

#include <stdlib.h>
#include <time.h>

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
    int dummyTemp;
    int dummySpeed;
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

    Motor motor(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, 19);

    if(!motor.init()) 
    {
        return;
    }

    srand(time(NULL));

    ESP_NOW esp_now;

    // Creating Dummy Data to send to the controller 
    carData carData;

    carData.dummySpeed = 0;
    carData.dummyTemp = 20;


    // ESP_Now 
    esp_now.start(onDataSend, onDataReceived);
    esp_now.addNewESP(macAddr);

    while(1) 
    {

        motor.setSpeed(receivedData.xValue);

        carData.dummySpeed = rand() % 201;
        carData.dummyTemp = (rand() % 51) - 10;


        // Send Dummy Data to Controller every Second
        esp_now.sendData(macAddr, &carData, sizeof(carData));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
