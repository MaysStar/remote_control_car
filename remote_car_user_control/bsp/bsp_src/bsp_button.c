#include "bsp_button.h"

void bsp_button_init(void)
{
    /* Configure GPIO nums */
    gpio_config_t gpio_config_user_control = 
    {
        .pin_bit_mask =    ((1ULL << BSP_BUTTON_FORWARD ) | 
                            (1ULL << BSP_BUTTON_BACKWARD ) | 
                            (1ULL << BSP_BUTTON_RIGHT ) | 
                            (1ULL << BSP_BUTTON_LEFT )),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&gpio_config_user_control));
}

bsp_button_pressed bsp_button_get_state(void)
{
    bsp_button_pressed curr_button_pressed = BSP_CAR_DIRECTION_STOP_BUTTON;

    if(gpio_get_level(BSP_BUTTON_FORWARD) == BSP_BUTTON_ACTIVE)
    {
        curr_button_pressed = BSP_CAR_DIRECTION_FORWARD_BUTTON;
    }
    else if(gpio_get_level(BSP_BUTTON_BACKWARD) == BSP_BUTTON_ACTIVE)
    {
        curr_button_pressed = BSP_CAR_DIRECTION_BACKWARD_BUTTON;
    }
    else if(gpio_get_level(BSP_BUTTON_RIGHT) == BSP_BUTTON_ACTIVE)
    {
        curr_button_pressed = BSP_CAR_DIRECTION_RIGHT_BUTTON;
    }
    else if(gpio_get_level(BSP_BUTTON_LEFT) == BSP_BUTTON_ACTIVE)
    {
        curr_button_pressed = BSP_CAR_DIRECTION_LEFT_BUTTON;
    }

    return curr_button_pressed;
}