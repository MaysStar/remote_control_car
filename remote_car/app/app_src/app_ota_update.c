#include "app_ota_update.h"
#include "app_global_state.h"
#include "esp_timer.h"
#include <string.h>

#define APP_OTA_STOP_TIMEOUT_MS 500U
#define APP_OTA_POLL_MS 10U
#define APP_OTA_NETWORK_TIMEOUT_MS 15000
#define APP_OTA_DOWNLOAD_TIMEOUT_US (120LL * 1000LL * 1000LL)
#define APP_OTA_BUFFER_SIZE 8192

esp_err_t app_ota_run_update(const char *url)
{
    app_request_motion_inhibit();
    const TickType_t stop_started = xTaskGetTickCount();
    while (!app_is_motor_stop_confirmed())
    {
        if ((TickType_t)(xTaskGetTickCount() - stop_started) >=
            pdMS_TO_TICKS(APP_OTA_STOP_TIMEOUT_MS))
        {
            return ESP_ERR_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(APP_OTA_POLL_MS));
    }

    if (url == NULL || strncmp(url, "https://", 8) != 0 || url[8] == '\0' || url[8] == '/')
    {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_http_client_config_t http_config = {
        .url = url,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .skip_cert_common_name_check = false,
        .disable_auto_redirect = true,
        .buffer_size = APP_OTA_BUFFER_SIZE,
        .buffer_size_tx = APP_OTA_BUFFER_SIZE,
        .timeout_ms = APP_OTA_NETWORK_TIMEOUT_MS,
    };
    const esp_https_ota_config_t ota_config = {.http_config = &http_config};
    esp_https_ota_handle_t handle = NULL;
    const int64_t started = esp_timer_get_time();
    esp_err_t result = esp_https_ota_begin(&ota_config, &handle);
    if (result != ESP_OK)
    {
        return result;
    }

    do
    {
        if (esp_timer_get_time() - started >= APP_OTA_DOWNLOAD_TIMEOUT_US)
        {
            esp_https_ota_abort(handle);
            return ESP_ERR_TIMEOUT;
        }
        result = esp_https_ota_perform(handle);
        if (result == ESP_ERR_HTTPS_OTA_IN_PROGRESS)
        {
            vTaskDelay(pdMS_TO_TICKS(APP_OTA_POLL_MS));
        }
    } while (result == ESP_ERR_HTTPS_OTA_IN_PROGRESS);

    if (esp_timer_get_time() - started >= APP_OTA_DOWNLOAD_TIMEOUT_US)
    {
        esp_https_ota_abort(handle);
        return ESP_ERR_TIMEOUT;
    }

    if (result != ESP_OK || !esp_https_ota_is_complete_data_received(handle))
    {
        esp_https_ota_abort(handle);
        return result == ESP_OK ? ESP_ERR_INVALID_STATE : result;
    }

    /* finish consumes the handle, including on failure; never abort it afterwards. */
    return esp_https_ota_finish(handle);
}

void app_ota_update_task(void *pvParameters)
{
    (void)pvParameters;
    bool attempted = false;
    while (1)
    {
        if (!attempted && osal_get_ota_state() == HTTPS_OTA_UPDATE_PRESENT)
        {
            attempted = true;
            const esp_err_t result = app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR);
            if (result == ESP_OK)
            {
                ESP_LOGI("APP_OTA", "Update verified; restarting");
                esp_restart();
            }
            else
            {
                ESP_LOGE("APP_OTA", "Update failed (%d); motion locked until reboot", result);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
