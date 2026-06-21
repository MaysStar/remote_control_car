#ifndef BSP_MQTT_H
#define BSP_MQTT_H

#include "bsp_config.h"
#include "mqtt_client.h"
#include "esp_crt_bundle.h"

typedef void (*mqtt_callback_fun)(void* event_handler_arg,
                                    esp_event_base_t event_base,
                                    int32_t event_id,
                                    void* event_data);

void bsp_mqtt_register_callback(void (*mqtt_callback_fun)(void* event_handler_arg,
                                                            esp_event_base_t event_base,
                                                            int32_t event_id,
                                                            void* event_data));

void bsp_mqtt_init(void);
void bsp_mqtt_subscribe_multiple(esp_mqtt_topic_t* topic_list, int size);

#endif /* BSP_MQTT_H */