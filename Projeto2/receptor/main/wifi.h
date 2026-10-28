#pragma once
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"

esp_err_t __wifi_init(void);
esp_err_t __wifi_connect(char* wifi_ssid, char* wifi_password);
esp_err_t __wifi_disconnect(void);
esp_err_t __wifi_deinit(void);
bool __wifi_is_connected(void);
void __wifi_event_cb(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void __ip_event_cb(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
