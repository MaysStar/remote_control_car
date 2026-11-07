#include "bsp_button.h"
#include "esp_timer.h"
#include <stdbool.h>
#include <stddef.h>

#define BSP_BUTTON_DEBOUNCE_US 30000LL

static bsp_button_pressed candidate = BSP_CAR_DIRECTION_STOP_BUTTON;
static int64_t candidate_since_us;
static bool release_required;

void bsp_button_init(void)
{
    gpio_config_t configuration = {
        .pin_bit_mask = ((1ULL << BSP_BUTTON_FORWARD) | (1ULL << BSP_BUTTON_BACKWARD) |
                         (1ULL << BSP_BUTTON_RIGHT) | (1ULL << BSP_BUTTON_LEFT)),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    candidate = BSP_CAR_DIRECTION_STOP_BUTTON;
    candidate_since_us = 0;
    release_required = false;
    ESP_ERROR_CHECK(gpio_config(&configuration));
}

bsp_button_pressed bsp_button_get_state(void)
{
    const gpio_num_t pins[] = {BSP_BUTTON_FORWARD, BSP_BUTTON_BACKWARD, BSP_BUTTON_RIGHT,
                               BSP_BUTTON_LEFT};
    const bsp_button_pressed directions[] = {
        BSP_CAR_DIRECTION_FORWARD_BUTTON, BSP_CAR_DIRECTION_BACKWARD_BUTTON,
        BSP_CAR_DIRECTION_RIGHT_BUTTON, BSP_CAR_DIRECTION_LEFT_BUTTON};
    bsp_button_pressed selected = BSP_CAR_DIRECTION_STOP_BUTTON;
    unsigned pressed_count = 0U;

    for (size_t index = 0; index < sizeof(pins) / sizeof(pins[0]); ++index)
    {
        if (gpio_get_level(pins[index]) == BSP_BUTTON_ACTIVE)
        {
            selected = directions[index];
            ++pressed_count;
        }
    }

    if (pressed_count > 1U)
    {
        release_required = true;
    }
    if (pressed_count == 0U)
    {
        release_required = false;
    }
    if (pressed_count == 0U || release_required)
    {
        candidate = BSP_CAR_DIRECTION_STOP_BUTTON;
        return BSP_CAR_DIRECTION_STOP_BUTTON;
    }

    const int64_t now_us = esp_timer_get_time();
    if (selected != candidate || now_us < candidate_since_us)
    {
        candidate = selected;
        candidate_since_us = now_us;
        return BSP_CAR_DIRECTION_STOP_BUTTON;
    }
    if (now_us - candidate_since_us < BSP_BUTTON_DEBOUNCE_US)
    {
        return BSP_CAR_DIRECTION_STOP_BUTTON;
    }
    return selected;
}
