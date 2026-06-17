#include "app_global_state.h"

/* Private variables and functions */
static volatile SemaphoreHandle_t m_net_ip_fl = NULL;
static volatile SemaphoreHandle_t m_pos = NULL;
static volatile SemaphoreHandle_t m_mqtt_com = NULL;

static volatile uint8_t net_state = APP_GLOBAL_STATE_NETWORK_OFF;
static volatile app_car_position car_position;
static volatile app_car_dir_speed car_dir_speed;

/* Initialize all state and data */
void app_global_state_init(void)
{
    m_net_ip_fl = xSemaphoreCreateMutex();
    configASSERT(m_net_ip_fl != NULL);

    m_pos = xSemaphoreCreateMutex();
    configASSERT(m_pos != NULL);

    m_mqtt_com = xSemaphoreCreateMutex();
    configASSERT(m_mqtt_com != NULL);
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

void app_set_position(app_car_position* curr_pos)
{
    if(m_pos != NULL)
    {
        xSemaphoreTake(m_pos, portMAX_DELAY);
        car_position = *curr_pos;
        xSemaphoreGive(m_pos);
    }
}

app_car_position app_get_position(void)
{
    app_car_position curr_pos = {};
    if(m_pos != NULL)
    {
        xSemaphoreTake(m_pos, portMAX_DELAY);
        curr_pos = car_position;
        xSemaphoreGive(m_pos);
    }

    return curr_pos;
}