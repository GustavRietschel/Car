#include "connections.h"
#include <iostream>

struct joystickData {
    int xValue;
    int yValue;
};

struct carData {
    int dummyTemp;
    int dummySpeed;
};

void mySendCallback(const uint8_t *macAddr, esp_now_send_status_t status) {
    if (status != ESP_NOW_SEND_SUCCESS) {
        printf("Senden fehlgeschlagen");
    }
}

void myReceiveCallback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len) {
    joystickData receivedData;
    if (len == sizeof(receivedData)) {
        memcpy(&receivedData, data, sizeof(receivedData));
        std::cout << "Empfangene Daten: X = " << receivedData.xValue << ", Y = " << receivedData.yValue << std::endl;
    } else {
        std::cout << "Falsche Datenlänge: " << len << " Bytes" << std::endl;
    }
}

uint8_t macAddr[6] = {0x12, 0x2b, 0x2f, 0xc4, 0xc9, 0x4c};
ESP_NOW esp_now;

extern "C" void app_main(void) 
{
    carData carData;

    carData.dummySpeed = 100;
    carData.dummyTemp = 30;

    esp_now.start(mySendCallback, myReceiveCallback);
    esp_now.addNewESP(macAddr);

    while(1) 
    {
        esp_now.sendData(macAddr, &carData, sizeof(carData));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
