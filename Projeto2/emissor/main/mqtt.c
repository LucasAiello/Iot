#include "mqtt.h"

#define TAG "::MQTT::"
static esp_mqtt_client_handle_t client = NULL;
static char* topic = "";
static char* addrs = "";

void __mqtt_set_addrs(char* newAddrs)
{
    addrs = newAddrs;
}

void __mqtt_set_topic(char* newTopic)
{
    topic = newTopic;
}

void __mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Conectado ao Broker MQTT!");
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Desconectado do Broker MQTT");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "Inscrição confirmada, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "CHEGOU");
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

void __mqtt_publish_data(const char *topic, const char *data)
{
    ESP_LOGI(TAG, "Mensagem recebida: %s", data);
    if (client != NULL) {
        esp_mqtt_client_publish(client, topic, data, 0, 0, 0);
    }
}