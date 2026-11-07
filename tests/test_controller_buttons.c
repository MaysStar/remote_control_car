#include "bsp_button.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static unsigned pressed;
static int64_t now_us;

int64_t esp_timer_get_time(void)
{
    return now_us;
}

esp_err_t gpio_config(const gpio_config_t *configuration)
{
    assert(configuration->mode == GPIO_MODE_INPUT);
    assert(configuration->pull_up_en == GPIO_PULLUP_ENABLE);
    assert(configuration->pin_bit_mask ==
           ((1ULL << 23) | (1ULL << 22) | (1ULL << 25) | (1ULL << 26)));
    return ESP_OK;
}

int gpio_get_level(gpio_num_t pin)
{
    const int pins[] = {23, 22, 25, 26};
    for (unsigned index = 0; index < 4; ++index)
    {
        if (pin == pins[index])
        {
            return (pressed & (1U << index)) ? 0 : 1;
        }
    }
    assert(0);
    return 1;
}

static void expect(unsigned mask, int64_t time_us, bsp_button_pressed expected)
{
    pressed = mask;
    now_us = time_us;
    assert(bsp_button_get_state() == expected);
}

int main(void)
{
    const bsp_button_pressed stop = BSP_CAR_DIRECTION_STOP_BUTTON;
    for (unsigned direction = 0; direction < 4; ++direction)
    {
        bsp_button_init();
        expect(0, 0, stop);
        expect(1U << direction, 1000, stop);
        expect(1U << direction, 30999, stop);
        expect(1U << direction, 31000, (bsp_button_pressed)direction);
        expect(1U << direction, 90000, (bsp_button_pressed)direction);
        expect(0, 90001, stop);
        expect(1U << direction, 90002, stop);
    }

    /* Every combination of conflicting buttons requires full release. */
    for (unsigned mask = 1; mask < 16; ++mask)
    {
        if ((mask & (mask - 1U)) == 0U)
        {
            continue;
        }
        bsp_button_init();
        expect(1, 0, stop);
        expect(1, 30000, BSP_CAR_DIRECTION_FORWARD_BUTTON);
        expect(mask, 30001, stop);
        expect(1, 100000, stop);
        expect(1, 200000, stop);
        expect(0, 200001, stop);
        expect(1, 200002, stop);
        expect(1, 230002, BSP_CAR_DIRECTION_FORWARD_BUTTON);
    }

    bsp_button_init();
    expect(1, 0, stop);
    expect(0, 10000, stop);
    expect(1, 20000, stop);
    expect(1, 49999, stop);
    expect(1, 50000, BSP_CAR_DIRECTION_FORWARD_BUTTON);
    expect(2, 50001, stop);
    expect(2, 80001, BSP_CAR_DIRECTION_BACKWARD_BUTTON);
    expect(2, 1, stop); /* Clock rollback restarts qualification. */
    expect(2, 30001, BSP_CAR_DIRECTION_BACKWARD_BUTTON);
    bsp_button_init();
    expect(2, 40000, stop); /* Reinitialization clears accepted input. */
    puts("controller button tests passed");
}
