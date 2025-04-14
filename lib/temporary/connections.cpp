#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "connections.h"


void ESP_NOW::wifiInit() 
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));
}


void ESP_NOW::start(onDataSend sendCb, onDataReceive receiveCb) {
   wifiInit();
   ESP_ERROR_CHECK(esp_now_init());
   esp_now_register_send_cb(sendCb);
   esp_now_register_recv_cb(receiveCb);
};

void ESP_NOW::addNewESP(uint8_t macAddr[6]) 
{
    esp_now_peer_info_t peer_info = {0};
    peer_info.channel = 1;
    peer_info.encrypt = false;
    memcpy(peer_info.peer_addr, macAddr, 6);
    esp_now_add_peer(&peer_info);
}

void ESP_NOW::sendData(uint8_t macAddr[6], const void* data, size_t len)
{
    esp_err_t result = esp_now_send(macAddr, reinterpret_cast<const uint8_t*>(data), len);
    if (result != ESP_OK) {
        // Fehlercode ausgeben
        printf("Fehler beim Senden: %s\n", esp_err_to_name(result));
    };
}