#include "app_mqtt_communication.h"

/* Private variables and functions */
static const char* TAG = "APP_MQTT";

static const char* topic_dir = "direction";
static const char* topic_speed = "speed";

static uint8_t prev_speed = 0;

static uint8_t subscribed_topics_count = 0;

static void app_mqtt_handler(void* event_handler_arg,
                        esp_event_base_t event_base,
                        int32_t event_id,
                        void* event_data);

static void app_mqtt_handler(void* event_handler_arg,
                        esp_event_base_t event_base,
                        int32_t event_id,
                        void* event_data)
{
    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            /* Connection is successful try to subscribe on direction and speed topics */
            esp_mqtt_topic_t topics[2];
            topics[0].filter = topic_dir;
            topics[0].qos = 0;

            topics[1].filter = topic_speed;
            topics[1].qos = 0;

            osal_mqtt_subscribe_multiple(topics, sizeof(topics) / sizeof(esp_mqtt_topic_t));

            break;

        case MQTT_EVENT_SUBSCRIBED:
            subscribed_topics_count++;
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "get data");
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

/* Main task to communicate with remote car*/
void app_mqtt_communication_task(void* pvParameters)
{
    while(1)
    {
        /* Get subscribed topics state and publish current user data */
        if(subscribed_topics_count == 1)
        {
            /* Transform dir and speed in char array */
            char dir = (char)(osal_button_get_state() + 48);
            osal_mqtt_publish(topic_dir, &dir, 1);

            char speed[4];

            /* Sending speed to remote car only, if it changed */
            uint8_t curr_speed = osal_potentiometer_adc_get_value();
            if(prev_speed != curr_speed)
            {
                /* Ten is decimal system of counting */
                itoa((int)curr_speed, speed, 10);
                osal_mqtt_publish(topic_speed, speed, strlen(speed));

                prev_speed = curr_speed;
            }
        }
        

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}