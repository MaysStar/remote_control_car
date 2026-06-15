#include "stdint.h"

#include "bsp_nvs_flash.h"
#include "bsp_log_hook.h"
#include "bsp_sntp.h"
#include "bsp_wifi.h"

#include "osal.h"

#include "app_wifi.h"
#include "app_udp_log.h"
#include "app_ota_update.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t app_udp_log_task_handle = NULL;
static TaskHandle_t app_ota_update_task_handle = NULL;

void app_main()
{
    /* Configure nvs flash */
    vTaskDelay(pdMS_TO_TICKS(2000));

    bsp_nvs_flash_start();

    /* Configure logs OTA update and global state */
    bsp_log_hook_init();

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    osal_init();
    bsp_https_ota_init();

    app_global_state_init();

    /* Start Wi-Fi. Use sntp and udp logs */
    app_wifi_init();
    bsp_wifi_start();

    /* Wait for Wi-Fi and IP got */
    while(app_get_network_flag() != APP_GLOBAL_STATE_NETWORK_ON)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    /* Board firmware is valid cansel rollback */
    const esp_partition_t* running_partition = esp_ota_get_running_partition();
    if(running_partition->subtype != ESP_PARTITION_SUBTYPE_APP_FACTORY)
    {
        ESP_ERROR_CHECK(esp_ota_mark_app_valid_cancel_rollback());
    }

    bsp_start_sntp_time(UA);

    xTaskCreate(app_udp_log_task, "app_udp_log_task", 4096, NULL, 1, &app_udp_log_task_handle);
    configASSERT(app_udp_log_task_handle != NULL);

    xTaskCreate(app_ota_update_task, "app_ota_update_task", 4096, NULL, 2, &app_ota_update_task_handle);
    configASSERT(app_ota_update_task_handle != NULL);

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}