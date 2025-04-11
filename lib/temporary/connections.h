#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <string.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <iostream>

class ESP_NOW 
{
    public:
        typedef void (*onDataReceive)(const esp_now_recv_info *esp_now_info, const uint8_t *data, int len);
        typedef void (*onDataSend)(const uint8_t *macAddr, esp_now_send_status_t status);
        void start(onDataSend sendCb, onDataReceive receiveCb);
        void addNewESP(uint8_t macAddr[6]);
        void sendData(uint8_t macAddr[6], const void* data, size_t len);

    private: 
        void wifiInit();
    
};

#endif