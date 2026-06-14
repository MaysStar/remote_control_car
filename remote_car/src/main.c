#include "stdint.h"

#include "bsp_nvs_flash.h"
#include "bsp_sntp.h"

#include "osal.h"

#include "app_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main()
{
    vTaskDelay(pdMS_TO_TICKS(2000)); 

    bsp_nvs_flash_start();

    app_wifi_init();

    vTaskDelay(pdMS_TO_TICKS(10000));

    bsp_start_sntp_time(UA);

    while(1)
    {
        ESP_LOGI("MAIN", "Hello! Board is running.");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
