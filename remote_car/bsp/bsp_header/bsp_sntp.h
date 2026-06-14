#ifndef BSP_SNTP_H
#define BSP_SNTP_H

#include "esp_err.h"
#include "esp_log.h"

#include "esp_netif_sntp.h"

typedef enum
{
    UA = 0,
    US,
}Contry_time_zone;

void bsp_start_sntp_time(Contry_time_zone tm_zn);

#endif /* BSP_SNTP_H */