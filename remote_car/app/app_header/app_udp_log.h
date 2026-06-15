#ifndef APP_UDP_LOG_H
#define APP_UDP_LOG_H

#include "esp_err.h"
#include "esp_log.h"

#include "socket.h"

#include "bsp_log_hook.h"
#include "bsp_config.h"

void app_udp_log_task(void* pvParameters);

#endif /* APP_UDP_LOG_H */