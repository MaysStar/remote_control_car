#ifndef TEST_CONTROLLER_MQTT_H
#define TEST_CONTROLLER_MQTT_H

#include <stdbool.h>
#include <stdint.h>
#include "freertos/task.h"

typedef const char *esp_event_base_t;
typedef void (*mqtt_callback_fun)(void *, esp_event_base_t, int32_t, void *);

enum
{
    MQTT_EVENT_CONNECTED,
    MQTT_EVENT_SUBSCRIBED,
    MQTT_EVENT_DATA,
    MQTT_EVENT_DISCONNECTED,
    MQTT_EVENT_ERROR
};

#define ESP_LOGE(tag, ...) ((void)(tag))

void bsp_mqtt_register_callback(mqtt_callback_fun callback);
uint8_t osal_button_get_state(void);
uint8_t osal_potentiometer_adc_get_value(void);
void osal_mqtt_publish(const char *topic, char *data, uint32_t length);
void app_mqtt_communication_init(void);
void app_mqtt_communication_task(void *parameters);
void vTaskDelay(TickType_t ticks);

#endif
