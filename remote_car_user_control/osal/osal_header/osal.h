#ifndef OSAL_H
#define OSAL_H

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "bsp_mqtt.h"

#include "bsp_button.h"
#include "bsp_potentiometer.h"

void osal_init(void);

void osal_mqtt_subscribe_multiple(esp_mqtt_topic_t* topic_list, int size);
void osal_mqtt_publish(const char *topic, char* data, uint32_t data_len);

bsp_button_pressed osal_button_get_state(void);
uint8_t osal_potentiometer_adc_get_value(void);

#endif /* OSAL_H */