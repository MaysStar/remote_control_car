#include "app_mqtt_communication.h"

/* Private variables and functions */
static const char* TAG = "APP_MQTT";

static void app_mqtt_handler(void* event_handler_arg,
                        esp_event_base_t event_base,
                        int32_t event_id,
                        void* event_data);

static void app_mqtt_handler(void* event_handler_arg,
                        esp_event_base_t event_base,
                        int32_t event_id,
                        void* event_data)
{
    esp_mqtt_event_t* esp_mqtt_event_data = (esp_mqtt_event_t*)event_data;

    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            /* Connection is successful try to subscribe on direction and speed topics */
            esp_mqtt_topic_t topics[2];
            topics[0].filter = "direction";
            topics[0].qos = 0;

            topics[1].filter = "speed";
            topics[1].qos = 0;

            osal_mqtt_subscribe_multiple(topics, sizeof(topics) / sizeof(esp_mqtt_topic_t));

            break;

        case MQTT_EVENT_DATA:
            char topic[APP_MQTT_TOPIC_DATA_LEN];
            char topic_data[APP_MQTT_TOPIC_DATA_LEN];

            snprintf(topic, APP_MQTT_TOPIC_DATA_LEN, "%.*s", esp_mqtt_event_data->topic_len, esp_mqtt_event_data->topic);
            snprintf(topic_data, APP_MQTT_TOPIC_DATA_LEN, "%.*s", esp_mqtt_event_data->data_len, esp_mqtt_event_data->data);

            if((strcmp(topic, "direction") == 0) && (esp_mqtt_event_data->data_len == 1))
            {
                uint8_t proposed_dir = (uint8_t)(topic_data[0] - 48);

                /* Direction filter  */
                if(proposed_dir <= 4)
                {
                    app_set_dir((app_car_dir_state)proposed_dir);
                }
                else
                {
                    ESP_LOGE(TAG, "impossible direction");                
                }                
            }

            else if(strcmp(topic, "speed")  == 0)
            {
                int proposed_speed = atoi(topic_data);

                /* Limitation for speed */
                proposed_speed = (proposed_speed < 0) ? 0 : proposed_speed;
                proposed_speed = (proposed_speed > 90) ? 90 : proposed_speed;

                app_set_speed((uint8_t)proposed_speed);
            }

            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGE(TAG, "disconnected, try to reconnect");
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "error, try to reconnect");
            break;

        default:
            break;
    }
}

void app_mqtt_communication_init(void)
{
    bsp_mqtt_register_callback(app_mqtt_handler);
}
