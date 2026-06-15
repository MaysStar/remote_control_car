#include "app_ota_update.h"

/* Private variables and functions */
static const char* TAG = "APP_OTA";

static esp_https_ota_handle_t esp_https_ota_handle = NULL;

void app_ota_update_task(void* pvParameters)
{
    while(1)
    {
        if(osal_get_ota_state() == HTTPS_OTA_UPDATE_PRESENT)
        {
            /* Configure https */
            esp_http_client_config_t esp_http_client_config = 
            {
                .url = HTTPS_OTA_UPDATE_IP_ADDR,

                /* Certificate for all websites */
                .crt_bundle_attach = esp_crt_bundle_attach,

                /* Make buffers size bigger to download firmware update */
                .buffer_size = 8192,
                .buffer_size_tx = 8192,
                .timeout_ms = 15000,
            };

            esp_https_ota_config_t esp_https_ota_config = 
            {
                .http_config = &esp_http_client_config,
            };

            /* Start OTA */
            esp_err_t err = esp_https_ota_begin(&esp_https_ota_config, &esp_https_ota_handle);

            if(err != ESP_OK)
            {
                ESP_LOGE(TAG, "begin error: %d", err);
                goto clean_up;
            }

            uint32_t ota_image_size = esp_https_ota_get_image_size(esp_https_ota_handle);

            /* Check OTA update status */
            while(esp_https_ota_perform(esp_https_ota_handle) == ESP_ERR_HTTPS_OTA_IN_PROGRESS)
            {
                uint32_t ota_update_in_percent = ((uint32_t)esp_https_ota_get_image_len_read(esp_https_ota_handle) * 100U / ota_image_size);
                ESP_LOGI(TAG, "update percent: %lu", ota_update_in_percent);
                vTaskDelay(pdMS_TO_TICKS(1));
            }

            err = esp_https_ota_finish(esp_https_ota_handle);

            if(err != ESP_OK)
            {
                ESP_LOGE(TAG, "finish error: %d", err);
                goto clean_up;
            }
            else 
            {
                ESP_LOGI(TAG, "update is successful, start reboot");
                esp_restart();
            }
        }

clean_up:

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}