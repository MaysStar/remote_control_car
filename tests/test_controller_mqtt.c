#include "app_mqtt_communication.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static mqtt_callback_fun handler;
static jmp_buf task_finished;
static unsigned published;
static unsigned expected_messages;
static unsigned input_reads;
static uint8_t direction;
static uint8_t speed;
static bool disconnect_after_direction;

void bsp_mqtt_register_callback(mqtt_callback_fun callback)
{
    handler = callback;
}

uint8_t osal_button_get_state(void)
{
    ++input_reads;
    return direction;
}

uint8_t osal_potentiometer_adc_get_value(void)
{
    ++input_reads;
    return speed;
}

void osal_mqtt_publish(const char *topic, char *data, uint32_t length)
{
    if (published == 0U)
    {
        assert(strcmp(topic, "direction") == 0);
        assert(length == 1U && data[0] == (char)('0' + direction));
        if (disconnect_after_direction)
        {
            handler(NULL, NULL, MQTT_EVENT_DISCONNECTED, NULL);
        }
    }
    else
    {
        char expected[4];
        const int size = snprintf(expected, sizeof(expected), "%u", (unsigned)speed);
        assert(strcmp(topic, "speed") == 0);
        assert(length == (uint32_t)size);
        assert(memcmp(data, expected, length) == 0);
    }
    ++published;
}

void vTaskDelay(TickType_t ticks)
{
    assert(ticks == pdMS_TO_TICKS(100));
    assert(published == expected_messages);
    assert(input_reads == expected_messages);
    longjmp(task_finished, 1);
}

static void poll(unsigned messages)
{
    published = 0U;
    input_reads = 0U;
    expected_messages = messages;
    if (setjmp(task_finished) == 0)
    {
        app_mqtt_communication_task(NULL);
    }
}

int main(void)
{
    app_mqtt_communication_init();
    assert(handler != NULL);
    poll(0);
    handler(NULL, NULL, MQTT_EVENT_SUBSCRIBED, NULL);
    poll(0);
    handler(NULL, NULL, MQTT_EVENT_CONNECTED, NULL);
    for (unsigned value = 0; value <= 100; ++value)
    {
        direction = (uint8_t)(value % 5U);
        speed = (uint8_t)value;
        poll(2);
    }
    handler(NULL, NULL, MQTT_EVENT_DISCONNECTED, NULL);
    poll(0);
    handler(NULL, NULL, MQTT_EVENT_SUBSCRIBED, NULL);
    poll(0);
    handler(NULL, NULL, MQTT_EVENT_CONNECTED, NULL);
    direction = 4;
    speed = 100;
    poll(2);
    handler(NULL, NULL, MQTT_EVENT_ERROR, NULL);
    poll(0);
    handler(NULL, NULL, MQTT_EVENT_CONNECTED, NULL);
    disconnect_after_direction = true;
    poll(2); /* A transport loss can occur between the two publishes. */
    disconnect_after_direction = false;
    poll(0);
    handler(NULL, NULL, MQTT_EVENT_CONNECTED, NULL);
    poll(2);
    app_mqtt_communication_init();
    poll(0);
    puts("controller MQTT task tests passed");
}
