#ifndef APP_OTA_UPDATE_H
#define APP_OTA_UPDATE_H

#include "bsp_config.h"

#include "osal.h"

#include "esp_crt_bundle.h"
#include "esp_ota_ops.h"

void app_ota_update_task(void* pvParameters);

#endif /* APP_OTA_UPDATE_H */