#include "mqtt.h"
#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#define LED_PIN GPIO_NUM_4
#define TAG "::MQTT::" 

static esp_mqtt_client_handle_t client = NULL;
static char topic[128] = {0};
static char addrs[128] = {0};

static char mqtt_data[256] = {0};

void __mqtt_set_addrs(char* newAddrs)
{
    if (newAddrs != NULL) {
        strncpy(addrs, newAddrs, sizeof(addrs) - 1);
    }
}

void __mqtt_set_topic(char* newTopic)
{
    if (newTopic != NULL) {
        strncpy(topic, newTopic, sizeof(topic) - 1);
    }
}

void __mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Conectado ao Broker MQTT!");
        
        if (strlen(topic) > 0) {
            int msg_id = esp_mqtt_client_subscribe(client, topic, 0);
        }
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Desconectado do Broker MQTT");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "Inscrição confirmada, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        memset(mqtt_data, 0, sizeof(mqtt_data));
        
        int len_to_copy = event->data_len < (sizeof(mqtt_data) - 1) ? event->data_len : (sizeof(sizeof(mqtt_data) - 1));
        memcpy(mqtt_data, event->data, len_to_copy);

        if (strncmp(mqtt_data, "BAT_SIGNAL_ON", event->data_len) == 0) {
            gpio_set_level(LED_PIN, 1);
            ESP_LOGI(TAG, "[ALERTA] Bat-Sinal Ativado! O Cavaleiro das Trevas foi convocado.");
        } 
        else if (strncmp(mqtt_data, "BAT_SIGNAL_OFF", event->data_len) == 0) {
            gpio_set_level(LED_PIN, 0);
            ESP_LOGI(TAG, "[INFO] Bat-Sinal Desativado.");
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "Erro no cliente MQTT");
        break;

    default:
        break;
    }
}

void __mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = addrs,
        .session = {
            .keepalive = 60,
            .disable_clean_session = false,
        },
    };

    client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, __mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void __mqtt_publish_data(const char *pub_topic, const char *data)
{
    if (client != NULL && data != NULL) {
        esp_mqtt_client_publish(client, pub_topic, data, 0, 0, 0); 
    }
}