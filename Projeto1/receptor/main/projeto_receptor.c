#include <stdio.h>
#include "wifi.h"
#include "mqtt.h"
#include "freertos/freeRTOS.h"
#include "driver/gpio.h"
#define WIFI_SSID "MERCUSYS_7E02"
#define WIFI_PASSWORD "70960594"
#define MQTT_ADDRS "mqtt://192.168.1.107:1883"
#define MQTT_TOPIC "gotham/dpgc/lucas&luiza/batsignal"
#define LED_PIN GPIO_NUM_4
#define TAG "::MAIN::"

volatile bool isPressed = false;
volatile bool isOn = false;

void init()
{
    gpio_config_t push_bttn_config = {
        .pin_bit_mask = 1UL << LED_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&push_bttn_config);
}

void app_main(void)
{
    init();
    __wifi_init();
    __wifi_connect(WIFI_SSID, WIFI_PASSWORD);
    if(__wifi_is_connected())
    {
        __mqtt_set_addrs(MQTT_ADDRS);
        __mqtt_set_topic(MQTT_TOPIC);
        __mqtt_app_start();
    }
}
