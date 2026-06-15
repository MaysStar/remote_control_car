#include "osal.h"

/* Private variables and functions */
static volatile SemaphoreHandle_t m_ota_ud_fl = NULL;

void osal_init(void)
{
    m_ota_ud_fl = xSemaphoreCreateMutex();
    configASSERT(m_ota_ud_fl != NULL);
}

/* Thread-safe get ota state function */
uint8_t osal_get_ota_state(void)
{
    if(m_ota_ud_fl != NULL)
    {
        uint8_t osal_ota_state = HTTPS_OTA_UPDATE_NOT_PRESENT;

        xSemaphoreTake(m_ota_ud_fl, portMAX_DELAY);
        osal_ota_state = bsp_get_ota_state();
        xSemaphoreGive(m_ota_ud_fl);

        return osal_ota_state;
    }

    return HTTPS_OTA_UPDATE_NOT_PRESENT;
}