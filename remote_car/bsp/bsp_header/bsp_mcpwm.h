#ifndef BSP_MCPWM_H
#define BSP_MCPWM_H

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/mcpwm_prelude.h"

#define BSP_MCPWM_LEFT_PIN_A    GPIO_NUM_4
#define BSP_MCPWM_LEFT_PIN_B    GPIO_NUM_5
#define BSP_MCPWM_RIGHT_PIN_A   GPIO_NUM_7
#define BSP_MCPWM_RIGHT_PIN_B   GPIO_NUM_6

#define BSP_MCPWM_TIMER_UPDATE_ON_EMPTY true
#define BSP_MCPWM_TIMER_PERIOD_TICKS 1000 // 20kHz 
#define BSP_MCPWM_TIMER_GROUP_ID 0
#define BSP_MCPWM_TIMER_INTR_PRIORITY 0

#define BSP_MCPWM_OPERATOR_UPDATE_ON_ZERO true
#define BSP_MCPWM_OPERATOR_GROUP_ID 0
#define BSP_MCPWM_OPERATOR_INTR_PRIORITY 0

#define BSP_MCPWM_COMPARATOR_UPDATE_ON_ZERO true
#define BSP_MCPWM_COMPARATOR_INTR_PRIORITY 0

void bsp_mcpwm_init(void);

void bsp_mcpwm_forward(uint32_t left_percent, uint32_t right_percent);
void bsp_mcpwm_backward(uint32_t left_percent, uint32_t right_percent);
void bsp_mcpwm_right(uint32_t speed);
void bsp_mcpwm_left(uint32_t speed);

#endif /* BSP_MCPWM_H */