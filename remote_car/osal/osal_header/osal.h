#ifndef OSAL_H
#define OSAL_H

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "bsp_ota_update.h"
#include "bsp_mpu6050.h"
#include "bsp_mqtt.h"
#include "bsp_http_pos.h"

void osal_init(void);

uint8_t osal_get_ota_state(void);
esp_err_t osal_mpu6050_get_angle(bsp_mpu6050_angle* curr_angle);
void osal_mqtt_subscribe_multiple(esp_mqtt_topic_t* topic_list, int size);

void osal_http_post_pos(float roll, float pitch, float yaw);
void osal_http_cleanup(void);

#endif /* OSAL_H */