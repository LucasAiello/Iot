#pragma once
#include "mqtt_client.h"
#include "esp_log.h"
void __mqtt_set_addrs(char* newAddrs);
void __mqtt_set_topic(char* newTopic);
void __mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void __mqtt_app_start(void);
void __mqtt_publish_data(const char *topic, const char *data);