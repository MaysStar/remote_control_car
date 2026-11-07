#include "app_mqtt_communication.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

/* Private variables and functions */
static const char *TAG = "APP_MQTT";

static const char *topic_dir = "direction";
static const char *topic_speed = "speed";

static atomic_bool is_mqtt_connected = ATOMIC_VAR_INIT(false);

static void app_mqtt_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                             void *event_data);

static void app_mqtt_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                             void *event_data)
{
    (void)event_handler_arg;
    (void)event_base;
    (void)event_data;

    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            atomic_store(&is_mqtt_connected, true);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGE(TAG, "disconnected, try to reconnect");
            atomic_store(&is_mqtt_connected, false);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "error, try to reconnect");
            atomic_store(&is_mqtt_connected, false);
            break;

        default:
            break;
    }
}

void app_mqtt_communication_init(void)
{
    atomic_store(&is_mqtt_connected, false);
    bsp_mqtt_register_callback(app_mqtt_handler);
}

/* Main task to communicate with remote car*/
void app_mqtt_communication_task(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        /* The controller publishes commands as soon as the transport connects. */
        if (atomic_load(&is_mqtt_connected))
        {
            /* Transform dir and speed in char array */
            char dir = (char)(osal_button_get_state() + '0');
            osal_mqtt_publish(topic_dir, &dir, 1);

            char speed[4];

            /* Refresh both fields every 100 ms, including after reconnect. */
            uint8_t curr_speed = osal_potentiometer_adc_get_value();
            snprintf(speed, sizeof(speed), "%u", (unsigned)curr_speed);
            osal_mqtt_publish(topic_speed, speed, strlen(speed));
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}