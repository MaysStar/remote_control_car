#ifndef OSAL_H
#define OSAL_H

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "bsp_ota_update.h"

void osal_init(void);

uint8_t osal_get_ota_state(void);


#endif /* OSAL_H */