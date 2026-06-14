#include "bsp_nvs_flash.h"

void bsp_nvs_flash_start(void)
{
    /* 
     * Initialize non-voltage flash 
     */
    esp_err_t err = nvs_flash_init();

    if((err == ESP_ERR_NVS_NO_FREE_PAGES)  || (err == ESP_ERR_NVS_NEW_VERSION_FOUND))
    {
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Reinit */
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
}