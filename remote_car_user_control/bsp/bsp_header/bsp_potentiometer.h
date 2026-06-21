#ifndef BSP_POTENTIOMETER_H
#define BSP_POTENTIOMETER_H

#include "stdint.h"

#include "esp_err.h"
#include "esp_log.h"

#include "esp_adc/adc_continuous.h"
#include "soc/adc_channel.h"
#include "hal/adc_types.h"

#define BSP_POTENTIOMETER_ADC_ATTEN ADC_ATTEN_DB_12
#define BSP_POTENTIOMETER_ADC_CHANNEL ADC1_GPIO32_CHANNEL
#define BSP_POTENTIOMETER_ADC_UNIT ADC_UNIT_1
#define BSP_POTENTIOMETER_ADC_BIT_WIDTH ADC_BITWIDTH_12

#define BSP_POTENTIOMETER_ADC_BIT_VALUE 4095.0f

void bsp_potentiometer_adc_init(void);
uint8_t bsp_potentiometer_adc_get_value(void);

#endif /* BSP_POTENTIOMETER_H */