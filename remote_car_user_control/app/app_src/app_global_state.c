#include "app_global_state.h"

/* Private variables and functions */
static volatile SemaphoreHandle_t m_net_ip_fl = NULL;
static volatile SemaphoreHandle_t m_mqtt_topics = NULL;

static volatile uint8_t net_state = APP_GLOBAL_STATE_NETWORK_OFF;

static volatile uint32_t mqtt_topics = 0;

/* Initialize all state and data */
void app_global_state_init(void)
{
    m_net_ip_fl = xSemaphoreCreateMutex();
    configASSERT(m_net_ip_fl != NULL);

    m_mqtt_topics = xSemaphoreCreateMutex();
    configASSERT(m_mqtt_topics != NULL);
}

void app_set_network_flag(uint8_t new_net_fl)
{
    if(m_net_ip_fl != NULL)
    {
        xSemaphoreTake(m_net_ip_fl, portMAX_DELAY);
        net_state = new_net_fl;
        xSemaphoreGive(m_net_ip_fl);
    }
}

uint8_t app_get_network_flag(void)
{
    if(m_net_ip_fl != NULL)
    {
        uint8_t temp_state;
        xSemaphoreTake(m_net_ip_fl, portMAX_DELAY);
        temp_state = net_state;
        xSemaphoreGive(m_net_ip_fl);
        return temp_state;
    }

    return 0;
}