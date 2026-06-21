#include "esp_err.h"
#include "esp_log.h"

#include "bsp_nvs_flash.h"
#include "bsp_wifi.h"
#include "bsp_mqtt.h"

#include "bsp_button.h"
#include "bsp_potentiometer.h"

#include "osal.h"

#include "app_global_state.h"
#include "app_wifi.h"
#include "app_mqtt_communication.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t app_mqtt_communication_task_handle = NULL;

void app_main() {
    /* Configure NVS flash */
    bsp_nvs_flash_start();

    /* Configure user communication GPIO buttons */
    bsp_button_init();

    /* OSAL init */
    osal_init();

    /* Initialize app state to get Wi-Fi flag */
    app_global_state_init();

    /* Configure Wi-Fi and callback */
    app_wifi_init();
    bsp_wifi_start();

    /* Wait for Wi-Fi flag */
    while(app_get_network_flag() != APP_GLOBAL_STATE_NETWORK_ON)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    /* Initialize ADC potentiometer after phy and Wi-Fi */
    bsp_potentiometer_adc_init();

    /* Configure MQTT communication and callback */
    app_mqtt_communication_init();
    bsp_mqtt_init();

    xTaskCreate(app_mqtt_communication_task, "app_mqtt_communication_task", 8192, NULL, 3, &app_mqtt_communication_task_handle);
    configASSERT(app_mqtt_communication_task_handle != NULL);

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}