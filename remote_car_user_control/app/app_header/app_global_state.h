#ifndef APP_GLOBAL_STATE_H
#define APP_GLOBAL_STATE_H

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define APP_GLOBAL_STATE_NETWORK_OFF    0U
#define APP_GLOBAL_STATE_NETWORK_ON     1U

#define APP_GLOBAL_TOPIC_DIR_OFF        0U
#define APP_GLOBAL_TOPIC_DIR_ON         1U
#define APP_GLOBAL_TOPIC_SPEED_OFF      2U
#define APP_GLOBAL_TOPIC_SPEED_ON       3U

typedef enum{
    APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD = 0,
    APP_MQTT_COMMUNICATION_CAR_DIRECTION_BACKWARD,
    APP_MQTT_COMMUNICATION_CAR_DIRECTION_RIGHT,
    APP_MQTT_COMMUNICATION_CAR_DIRECTION_LEFT,
    APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP,
}app_car_dir_state;

/* The state can only be used by applications */

void app_global_state_init(void);

void app_set_network_flag(uint8_t new_net_fl);
uint8_t app_get_network_flag(void);

#endif /* APP_GLOBAL_STATE_H */