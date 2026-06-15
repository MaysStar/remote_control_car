#ifndef BSP_OTA_UPDATE_H
#define BSP_OTA_UPDATE_H

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "esp_https_ota.h"

#define HTTPS_OTA_UPDATE_NOT_PRESENT    0
#define HTTPS_OTA_UPDATE_PRESENT        1
#define HTTPS_OTA_GPIO_TRIGER_NUM GPIO_NUM_13

uint8_t bsp_get_ota_state(void);
void bsp_https_ota_init(void);

#endif /* BSP_OTA_UPDATE_H */