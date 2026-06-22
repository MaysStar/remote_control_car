#include "osal.h"

/* Private variables and functions */
static volatile SemaphoreHandle_t m_mqtt = NULL;
static volatile SemaphoreHandle_t m_btn = NULL;
static volatile SemaphoreHandle_t m_potmtr = NULL;

void osal_init(void)
{
    m_mqtt = xSemaphoreCreateMutex();
    configASSERT(m_mqtt != NULL);

    m_btn = xSemaphoreCreateMutex();
    configASSERT(m_btn != NULL);

    m_potmtr = xSemaphoreCreateMutex();
    configASSERT(m_potmtr != NULL);
}

/* Thread-safe subscribe on topic */
void osal_mqtt_subscribe_multiple(esp_mqtt_topic_t* topic_list, int size)
{
    if(m_mqtt != NULL)
    {
        xSemaphoreTake(m_mqtt, portMAX_DELAY);
        bsp_mqtt_subscribe_multiple(topic_list, size);
        xSemaphoreGive(m_mqtt);
    }
}

/* Thread-safe publish data */
void osal_mqtt_publish(const char *topic, char* data, uint32_t data_len)
{
    if(m_mqtt != NULL)
    {
        xSemaphoreTake(m_mqtt, portMAX_DELAY);
        bsp_mqtt_publish(topic, data, data_len);
        xSemaphoreGive(m_mqtt);
    }
}

/* Thread-safe geting buttons state and potentiometer value ( user control data ) */
bsp_button_pressed osal_button_get_state(void)
{
    bsp_button_pressed curr_button_pressed = 0;

    if(m_btn != NULL)
    {
        xSemaphoreTake(m_btn, portMAX_DELAY);
        curr_button_pressed = bsp_button_get_state();
        xSemaphoreGive(m_btn);
    }

    return curr_button_pressed;
}

uint8_t osal_potentiometer_adc_get_value(void)
{
    uint8_t curr_pot_value = 0;

    if(m_potmtr != NULL)
    {
        xSemaphoreTake(m_potmtr, portMAX_DELAY);
        curr_pot_value = bsp_potentiometer_adc_get_value();
        xSemaphoreGive(m_potmtr);
    }

    return curr_pot_value;
}