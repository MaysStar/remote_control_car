#ifndef BSP_BUTTON_H
#define BSP_BUTTON_H


#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"

#define BSP_BUTTON_ACTIVE 0

#define BSP_BUTTON_FORWARD GPIO_NUM_23
#define BSP_BUTTON_BACKWARD GPIO_NUM_22

#define BSP_BUTTON_RIGHT GPIO_NUM_25
#define BSP_BUTTON_LEFT GPIO_NUM_26

typedef enum{
    BSP_CAR_DIRECTION_FORWARD_BUTTON = 0,
    BSP_CAR_DIRECTION_BACKWARD_BUTTON,
    BSP_CAR_DIRECTION_RIGHT_BUTTON,
    BSP_CAR_DIRECTION_LEFT_BUTTON,
    BSP_CAR_DIRECTION_STOP_BUTTON,
}bsp_button_pressed;


void bsp_button_init(void);
bsp_button_pressed bsp_button_get_state(void);

#endif /* BSP_BUTTON_H */