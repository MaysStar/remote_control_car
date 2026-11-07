#ifndef TEST_GPIO_H
#define TEST_GPIO_H
#include <stdint.h>
#include "esp_err.h"

typedef int gpio_num_t;
#define GPIO_NUM_23 23
#define GPIO_NUM_22 22
#define GPIO_NUM_25 25
#define GPIO_NUM_26 26
#define GPIO_MODE_INPUT 1
#define GPIO_PULLUP_ENABLE 1
#define GPIO_PULLDOWN_DISABLE 0
#define GPIO_INTR_DISABLE 0

typedef struct
{
    uint64_t pin_bit_mask;
    int mode;
    int pull_up_en;
    int pull_down_en;
    int intr_type;
} gpio_config_t;

esp_err_t gpio_config(const gpio_config_t *configuration);
int gpio_get_level(gpio_num_t pin);
#endif
