#ifndef APP_OTA_UPDATE_H
#define APP_OTA_UPDATE_H

#include "bsp_config.h"

#include "osal.h"

#include "esp_crt_bundle.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_system.h"
#include "freertos/task.h"

/* One update attempt. Motion remains inhibited on both success and failure. */
esp_err_t app_ota_run_update(const char *url);
void app_ota_update_task(void *pvParameters);

#endif /* APP_OTA_UPDATE_H */