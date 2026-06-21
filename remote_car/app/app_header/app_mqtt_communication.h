#ifndef APP_MQTT_COMMUNICATION_H
#define APP_MQTT_COMMUNICATION_H

#define APP_MQTT_TOPIC_DATA_LEN 32U

#include "bsp_mqtt.h"

#include "osal.h"

#include "app_global_state.h"

void app_mqtt_communication_init(void);

#endif /* APP_MQTT_COMMUNICATION_H */