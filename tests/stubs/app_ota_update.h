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
#define ESP_ERR_NO_MEM 4
#define ESP_ERR_INVALID_SIZE 5
#define ESP_FAIL 6
#define HTTPS_OTA_UPDATE_PRESENT 1
#define HTTPS_OTA_UPDATE_IP_ADDR "https://updates.invalid/firmware.bin"
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGE(...) ((void)0)
typedef void *esp_http_client_handle_t;
typedef unsigned esp_ota_handle_t;
typedef struct
{
    uint32_t size;
} esp_partition_t;
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
int esp_crt_bundle_attach(void *);
esp_http_client_handle_t esp_http_client_init(const esp_http_client_config_t *);
esp_err_t esp_http_client_open(esp_http_client_handle_t, int);
int64_t esp_http_client_fetch_headers(esp_http_client_handle_t);
int esp_http_client_get_status_code(esp_http_client_handle_t);
int esp_http_client_read(esp_http_client_handle_t, char *, int);
bool esp_http_client_is_complete_data_received(esp_http_client_handle_t);
esp_err_t esp_http_client_close(esp_http_client_handle_t);
esp_err_t esp_http_client_cleanup(esp_http_client_handle_t);
const esp_partition_t *esp_ota_get_next_update_partition(const esp_partition_t *);
esp_err_t esp_ota_begin(const esp_partition_t *, size_t, esp_ota_handle_t *);
esp_err_t esp_ota_write(esp_ota_handle_t, const void *, size_t);
esp_err_t esp_ota_end(esp_ota_handle_t);
esp_err_t esp_ota_abort(esp_ota_handle_t);
esp_err_t esp_ota_set_boot_partition(const esp_partition_t *);
void vTaskDelay(TickType_t ticks);
int osal_get_ota_state(void);
void esp_restart(void);
esp_err_t app_ota_run_update(const char *url);
#endif
