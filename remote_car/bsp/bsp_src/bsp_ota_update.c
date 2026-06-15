#include "bsp_ota_update.h"

/* Private variables and functions */
static volatile uint8_t ota_update_present = HTTPS_OTA_UPDATE_NOT_PRESENT;
static void IRAM_ATTR ota_gpio_isr_handler(void* pvParameters);

static void IRAM_ATTR ota_gpio_isr_handler(void* pvParameters)
{
    ota_update_present = HTTPS_OTA_UPDATE_PRESENT;
}

uint8_t bsp_get_ota_state(void)
{
    uint8_t ota_update_present_copy = ota_update_present;
    return ota_update_present_copy;
}

void bsp_https_ota_init(void)
{
    /* Configure gpio pin for ota update */
    gpio_config_t ota_update_gpio_config = 
    {
        .pin_bit_mask = (1ULL << HTTPS_OTA_GPIO_TRIGER_NUM),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    ESP_ERROR_CHECK(gpio_config(&ota_update_gpio_config));

    /* Register gpio isr for OTA update */
    ESP_ERROR_CHECK(gpio_isr_handler_add(HTTPS_OTA_GPIO_TRIGER_NUM, ota_gpio_isr_handler, NULL));
}

