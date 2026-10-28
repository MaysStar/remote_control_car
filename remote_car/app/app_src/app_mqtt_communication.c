#include "app_mqtt_communication.h"
#include "app_command_parser.h"
#include <string.h>

/* Private variables and functions */
static const char *TAG = "APP_MQTT";
static const char direction_topic[] = "direction";
static const char speed_topic[] = "speed";

#define APP_COMMAND_PROTOCOL_MAX_SPEED 100U
#define APP_COMMAND_DIRECTION_LENGTH 1U

static void app_mqtt_process_command(const esp_mqtt_event_t *event)
{
    uint8_t value;
    /* Fragments and retained commands must never actuate the vehicle. */
    if (!event || event->retain || event->current_data_offset != 0 ||
        event->data_len != event->total_data_len || event->data_len <= 0 || !event->topic ||
        !event->data)
    {
        app_stop_motion();
        return;
    }
    if (event->topic_len == sizeof(direction_topic) - 1U &&
        memcmp(event->topic, direction_topic, sizeof(direction_topic) - 1U) == 0)
    {
        if (event->data_len == APP_COMMAND_DIRECTION_LENGTH &&
            app_parse_decimal(event->data, event->data_len,
                              APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, &value))
        {
            app_set_dir((app_car_dir_state)value);
        }
        else
        {
            app_stop_motion();
        }
    }
    else if (event->topic_len == sizeof(speed_topic) - 1U &&
             memcmp(event->topic, speed_topic, sizeof(speed_topic) - 1U) == 0)
    {
        if (app_parse_decimal(event->data, event->data_len, APP_COMMAND_PROTOCOL_MAX_SPEED, &value))
        {
            app_set_speed(value > APP_COMMAND_MAX_SPEED ? APP_COMMAND_MAX_SPEED : value);
        }
        else
        {
            app_stop_motion();
        }
    }
}

static void app_mqtt_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                             void *event_data);

static void app_mqtt_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                             void *event_data)
{
    esp_mqtt_event_t *esp_mqtt_event_data = (esp_mqtt_event_t *)event_data;

    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            app_stop_motion();
            /* Connection is successful try to subscribe on direction and speed topics */
            esp_mqtt_topic_t topics[2];
            topics[0].filter = direction_topic;
            topics[0].qos = 0;

            topics[1].filter = speed_topic;
            topics[1].qos = 0;

            osal_mqtt_subscribe_multiple(topics, sizeof(topics) / sizeof(esp_mqtt_topic_t));

            break;

        case MQTT_EVENT_DATA:
        {
            app_mqtt_process_command(esp_mqtt_event_data);
            break;
        }

        case MQTT_EVENT_DISCONNECTED:
            app_stop_motion();
            ESP_LOGE(TAG, "disconnected, try to reconnect");
            break;

        case MQTT_EVENT_ERROR:
            app_stop_motion();
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
