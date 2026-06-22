#ifndef BSP_HTTP_POS_H
#define BSP_HTTP_POS_H

#include "esp_http_client.h"
#include "esp_crt_bundle.h"

#include "bsp_config.h"

void bsp_http_pos_init(void);

void bsp_http_post_pos(float roll, float pitch, float yaw);
void bsp_http_cleanup(void);

#endif /* BSP_HTTP_POS_H */