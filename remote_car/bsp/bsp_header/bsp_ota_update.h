#ifndef BSP_OTA_UPDATE_H
#define BSP_OTA_UPDATE_H

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "esp_https_ota.h"

#define HTTPS_OTA_GPIO_TRIGER_NUM GPIO_NUM_13

void bsp_https_ota_init();

#endif /* BSP_OTA_UPDATE_H */