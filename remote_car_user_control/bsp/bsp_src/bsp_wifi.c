#include "bsp_wifi.h"

/* Private variables */
static volatile wifi_callback_fun wifi_ip_callback = NULL;

/* Register callback from APP layer to handle WI-Fi/IP event */
void bsp_wifi_register_callback(void (*wifi_ip_callback_fun)(void* event_handler_arg,
                                                        esp_event_base_t event_base,
                                                        int32_t event_id,
                                                        void* event_data))
{
    assert(wifi_ip_callback_fun != NULL);

    wifi_ip_callback = wifi_ip_callback_fun;
}

/* Default start of wi-fi with event loop and tcp/ip stack */
void bsp_wifi_start(void)
{
    assert(wifi_ip_callback != NULL);

    /* Create event loop and netif lwip */
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Create default Wi-Fi station */
    esp_netif_create_default_wifi_sta();

    /* Register APP wi-fi handler function */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_ip_callback, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, wifi_ip_callback, NULL, NULL));

    /* Initialize wi-fi and set mode station */
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    /* Configure SSID, password and authmode */
    wifi_config_t wifi_config = 
    {
        .sta.ssid = BSP_CONFIG_WIFI_SSID,
        .sta.password = BSP_CONFIG_WIFI_PASSWORD,
        .sta.threshold.authmode = BSP_CONFIG_WIFI_AUTH,
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    /* Start wi-fi */
    esp_wifi_start();
}