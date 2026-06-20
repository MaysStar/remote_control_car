#include "osal.h"

/* Private variables and functions */
static volatile SemaphoreHandle_t m_ota_ud_fl = NULL;
static volatile SemaphoreHandle_t m_i2c0 = NULL;

static volatile SemaphoreHandle_t m_http = NULL;

void osal_init(void)
{
    m_ota_ud_fl = xSemaphoreCreateMutex();
    configASSERT(m_ota_ud_fl != NULL);

    m_i2c0 = xSemaphoreCreateMutex();
    configASSERT(m_i2c0 != NULL);

    m_http = xSemaphoreCreateMutex();
    configASSERT(m_http != NULL);
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

/* Thread-safe get car angles in space function */
esp_err_t osal_mpu6050_get_angle(bsp_mpu6050_angle* curr_angle)
{
    esp_err_t ret = ESP_FAIL;
    if(m_i2c0 != NULL)
    {
        xSemaphoreTake(m_i2c0, portMAX_DELAY);
        ret = bsp_mpu6050_get_angle(curr_angle);
        xSemaphoreGive(m_i2c0);
    }

    return ret;
}

/* Thread-safe set car position in visualization server */
void osal_http_post_pos(float roll, float pitch, float yaw)
{
    if(m_http != NULL)
    {
        xSemaphoreTake(m_http, portMAX_DELAY);
        bsp_http_post_pos(roll, pitch, yaw);
        xSemaphoreGive(m_http);
    }
}

/* Thread-safe function to close HTTP chanel */
void osal_http_cleanup(void)
{
    if(m_http != NULL)
    {
        xSemaphoreTake(m_http, portMAX_DELAY);
        bsp_http_cleanup();
        xSemaphoreGive(m_http);
    }
}