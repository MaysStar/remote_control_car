#include "bsp_mqtt.h"

/* Private variables and functions */
static const char* TAG = "BSP_MQTT";

static mqtt_callback_fun mqtt_callback = NULL;
static esp_mqtt_client_handle_t esp_mqtt_client_handle = NULL;

void bsp_mqtt_register_callback(void (*mqtt_callback_fun)(void* event_handler_arg,
                                                            esp_event_base_t event_base,
                                                            int32_t event_id,
                                                            void* event_data))
{
    if(mqtt_callback_fun != NULL)
    {
        mqtt_callback = mqtt_callback_fun;
    }
}

void bsp_mqtt_init(void)
{
    /* Configure MQTT client */
    esp_mqtt_client_config_t esp_mqtt_client_config = 
    {   
        .broker.address.hostname = BSP_CONFIG_MQTT_HOSTNAME,
        .broker.address.port = BSP_CONFIG_MQTT_PORT,
        .broker.address.transport = MQTT_TRANSPORT_OVER_SSL,
        .broker.verification.skip_cert_common_name_check = BSP_CONFIG_MQTT_SKIP_COMMONAME,

        .credentials.username = BSP_CONFIG_MQTT_USERNAME,
        .credentials.authentication.password = BSP_CONFIG_MQTT_PASSWORD,

        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_mqtt_client_handle = esp_mqtt_client_init(&esp_mqtt_client_config);

    if(esp_mqtt_client_handle != NULL)
    {
        ESP_ERROR_CHECK(esp_mqtt_client_register_event(esp_mqtt_client_handle, MQTT_EVENT_ANY, mqtt_callback, NULL));

        ESP_ERROR_CHECK(esp_mqtt_client_start(esp_mqtt_client_handle));

        ESP_LOGI(TAG, "client created successful");
    }
    else 
    {
        ESP_LOGE(TAG, "client created not successful");
    }
}

void bsp_mqtt_subscribe_multiple(esp_mqtt_topic_t* topic_list, int size)
{
    int res = esp_mqtt_client_subscribe_multiple(esp_mqtt_client_handle, topic_list, size);

    if(res >= 0)
    {
        ESP_LOGI(TAG, "subscribe is successful code: %d", res);
    }
    else if(res == -1)
    {
        ESP_LOGE(TAG, "subscribe is failure: -1");
    }

    else if(res == -2)
    {
        ESP_LOGE(TAG, "subscribe isn't successful, full outbox");
    }
}