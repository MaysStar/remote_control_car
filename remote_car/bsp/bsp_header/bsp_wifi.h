#ifndef BSP_WIFI_H
#define BSP_WIFI_H

#include "assert.h"

#include "esp_err.h"
#include "esp_log.h"

#include "esp_wifi.h"

#include "bsp_config.h"

void bsp_register_callback(void (*wifi_ip_callback_fun)(void* event_handler_arg,
                                                        esp_event_base_t event_base,
                                                        int32_t event_id,
                                                        void* event_data));

void bsp_wifi_start(void);

#endif /* BSP_WIFI_H */