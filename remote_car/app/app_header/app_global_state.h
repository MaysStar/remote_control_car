#ifndef APP_GLOBAL_STATE_H
#define APP_GLOBAL_STATE_H

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define APP_GLOBAL_STATE_NETWORK_OFF    0U
#define APP_GLOBAL_STATE_NETWORK_ON     1U

/* The state can only be used by applications */

typedef struct
{
    float roll;
    float pitch;
    float yaw;
}app_car_position;

typedef struct
{
    uint8_t speed;
    uint8_t direction;
}app_car_dir_speed;

void app_global_state_init(void);

void app_set_network_flag(uint8_t new_net_fl);
uint8_t app_get_network_flag(void);

void app_set_position(app_car_position* curr_pos);
app_car_position app_get_position(void);

void app_set_dir_speed(app_car_dir_speed* curr_dir_pos);
app_car_position app_get_position(void);

#endif /* APP_GLOBAL_STATE_H */