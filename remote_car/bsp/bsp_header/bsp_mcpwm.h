
#ifndef BSP_MCPWM_H
#define BSP_MCPWM_H

#include <stdint.h>

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/mcpwm_prelude.h"

/* PWM channels */
#define BSP_MCPWM_PHASE_A_LEFT_PWM       GPIO_NUM_4
#define BSP_MCPWM_PHASE_A_RIGHT_PWM      GPIO_NUM_5

#define BSP_MCPWM_PHASE_B_LEFT_PWM       GPIO_NUM_1
#define BSP_MCPWM_PHASE_B_RIGHT_PWM      GPIO_NUM_2

/* Motor direction channels - Phase A */
#define BSP_MCPWM_PHASE_A_LEFT_FORWARD   GPIO_NUM_6
#define BSP_MCPWM_PHASE_A_LEFT_BACKWARD  GPIO_NUM_7

#define BSP_MCPWM_PHASE_A_RIGHT_FORWARD  GPIO_NUM_16
#define BSP_MCPWM_PHASE_A_RIGHT_BACKWARD GPIO_NUM_15

/* Motor direction channels - Phase B */
#define BSP_MCPWM_PHASE_B_LEFT_FORWARD   GPIO_NUM_39
#define BSP_MCPWM_PHASE_B_LEFT_BACKWARD  GPIO_NUM_40

#define BSP_MCPWM_PHASE_B_RIGHT_FORWARD  GPIO_NUM_41
#define BSP_MCPWM_PHASE_B_RIGHT_BACKWARD GPIO_NUM_42

/* MCPWM timer configuration */
#define BSP_MCPWM_TIMER_UPDATE_ON_EMPTY true
#define BSP_MCPWM_TIMER_PERIOD_TICKS    20000
#define BSP_MCPWM_TIMER_RESOLUTION_HZ   20000000
#define BSP_MCPWM_TIMER_GROUP_ID        0
#define BSP_MCPWM_TIMER_INTR_PRIORITY   0

/* MCPWM operator configuration */
#define BSP_MCPWM_OPERATOR_UPDATE_ON_ZERO true
#define BSP_MCPWM_OPERATOR_GROUP_ID       0
#define BSP_MCPWM_OPERATOR_INTR_PRIORITY  0

/* MCPWM comparator configuration */
#define BSP_MCPWM_COMPARATOR_UPDATE_ON_ZERO true
#define BSP_MCPWM_COMPARATOR_INTR_PRIORITY  0

void bsp_mcpwm_init(void);

void bsp_mcpwm_forward(uint32_t speed_percent);
void bsp_mcpwm_backward(uint32_t speed_percent);
void bsp_mcpwm_right(uint32_t speed_percent);
void bsp_mcpwm_left(uint32_t speed_percent);
void bsp_mcpwm_stop(void);

#endif /* BSP_MCPWM_H */
