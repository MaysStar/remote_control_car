#include "app_ota_update.h"
#include "app_global_state.h"
#include "esp_timer.h"
#include <string.h>
#include <stdlib.h>

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

    esp_http_client_handle_t client = esp_http_client_init(&http_config);
    if (client == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    esp_ota_handle_t update = 0;
    bool update_started = false;
    char *buffer = NULL;
    const int64_t started = esp_timer_get_time();
    esp_err_t result = esp_http_client_open(client, 0);
    if (result != ESP_OK)
    {
        goto cleanup;
    }

    /* Streaming APIs do not follow redirects. Accept only a complete 200 response
       with a known positive length, before allocating or erasing flash. */
    const int64_t image_size = esp_http_client_fetch_headers(client);
    const esp_partition_t *partition = esp_ota_get_next_update_partition(NULL);
    if (esp_http_client_get_status_code(client) != 200 || image_size <= 0 || partition == NULL ||
        image_size > partition->size)
    {
        result = ESP_ERR_INVALID_SIZE;
        goto cleanup;
    }

    if (esp_timer_get_time() - started >= APP_OTA_DOWNLOAD_TIMEOUT_US)
    {
        result = ESP_ERR_TIMEOUT;
        goto cleanup;
    }
    buffer = malloc(APP_OTA_BUFFER_SIZE);
    if (buffer == NULL)
    {
        result = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    result = esp_ota_begin(partition, (size_t)image_size, &update);
    if (result != ESP_OK)
    {
        goto cleanup;
    }
    update_started = true;

    int64_t received = 0;
    while (received < image_size)
    {
        if (esp_timer_get_time() - started >= APP_OTA_DOWNLOAD_TIMEOUT_US)
        {
            result = ESP_ERR_TIMEOUT;
            goto cleanup;
        }
        const int requested = image_size - received < APP_OTA_BUFFER_SIZE
                                  ? (int)(image_size - received)
                                  : APP_OTA_BUFFER_SIZE;
        const int count = esp_http_client_read(client, buffer, requested);
        if (esp_timer_get_time() - started >= APP_OTA_DOWNLOAD_TIMEOUT_US)
        {
            result = ESP_ERR_TIMEOUT;
            goto cleanup;
        }
        if (count <= 0 || count > requested)
        {
            result = ESP_FAIL;
            goto cleanup;
        }
        result = esp_ota_write(update, buffer, (size_t)count);
        if (result != ESP_OK)
        {
            goto cleanup;
        }
        received += count;
        vTaskDelay(pdMS_TO_TICKS(APP_OTA_POLL_MS));
    }

    if (esp_timer_get_time() - started >= APP_OTA_DOWNLOAD_TIMEOUT_US)
    {
        result = ESP_ERR_TIMEOUT;
        goto cleanup;
    }
    if (!esp_http_client_is_complete_data_received(client))
    {
        result = ESP_ERR_INVALID_STATE;
        goto cleanup;
    }

    /* end consumes the handle even if image validation fails. */
    result = esp_ota_end(update);
    update_started = false;
    if (result == ESP_OK)
    {
        result = esp_ota_set_boot_partition(partition);
    }

cleanup:
    if (update_started)
    {
        esp_ota_abort(update);
    }
    free(buffer);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return result;
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
