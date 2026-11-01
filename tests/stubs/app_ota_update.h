#ifndef TEST_APP_OTA_UPDATE_H
#define TEST_APP_OTA_UPDATE_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "freertos/task.h"
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_ERR_TIMEOUT 1
#define ESP_ERR_INVALID_ARG 2
#define ESP_ERR_INVALID_STATE 3
#define ESP_ERR_HTTPS_OTA_IN_PROGRESS 4
#define HTTPS_OTA_UPDATE_PRESENT 1
#define HTTPS_OTA_UPDATE_IP_ADDR "https://updates.invalid/firmware.bin"
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGE(...) ((void)0)
typedef void *esp_https_ota_handle_t;
typedef struct
{
    const char *url;
    int (*crt_bundle_attach)(void *);
    bool skip_cert_common_name_check;
    bool disable_auto_redirect;
    int buffer_size;
    int buffer_size_tx;
    int timeout_ms;
} esp_http_client_config_t;
typedef struct
{
    const esp_http_client_config_t *http_config;
} esp_https_ota_config_t;
int esp_crt_bundle_attach(void *config);
esp_err_t esp_https_ota_begin(const esp_https_ota_config_t *, esp_https_ota_handle_t *);
esp_err_t esp_https_ota_perform(esp_https_ota_handle_t);
bool esp_https_ota_is_complete_data_received(esp_https_ota_handle_t);
esp_err_t esp_https_ota_finish(esp_https_ota_handle_t);
esp_err_t esp_https_ota_abort(esp_https_ota_handle_t);
void vTaskDelay(TickType_t ticks);
int osal_get_ota_state(void);
void esp_restart(void);
esp_err_t app_ota_run_update(const char *url);
#endif
