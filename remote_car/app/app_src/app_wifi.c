#include "app_wifi.h"

/* Priavte variables and functions */
static const char* TAG = "APP_WIFI";

static void app_wifi_handler(void* event_handler_arg,
                            esp_event_base_t event_base,
                            int32_t event_id,
                            void* event_data);

/* Wifi and ip event handler */
static void app_wifi_handler(void* event_handler_arg,
                            esp_event_base_t event_base,
                            int32_t event_id,
                            void* event_data)
{
    if(event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "wifi start try to connect");
                ESP_ERROR_CHECK(esp_wifi_connect());
                break;

            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "wifi connected");
                break;

            case WIFI_EVENT_STA_DISCONNECTED:

                /* Set net off in global state */
                app_set_network_flag(APP_GLOBAL_STATE_NETWORK_OFF);

                ESP_LOGE(TAG, "wifi disconnected");

                esp_err_t err = esp_wifi_connect();
                if (err != ESP_OK && err != ESP_ERR_WIFI_NOT_STARTED) {
                    ESP_LOGE(TAG, "Connection failed: %d", err);
                }

                break;
            default:
                break;
        }
    }
    else if(event_base == IP_EVENT)
    {
        switch (event_id)
        {
            case IP_EVENT_STA_GOT_IP:
                /* Set net on in global state */
                app_set_network_flag(APP_GLOBAL_STATE_NETWORK_ON);

                ip_event_got_ip_t* ip_data = (ip_event_got_ip_t*)event_data;
                ESP_LOGI(TAG, "event got ip: "IPSTR, IP2STR(&ip_data->ip_info.ip));
                break;
            
            case IP_EVENT_STA_LOST_IP:
                /* Set net off in global state */
                app_set_network_flag(APP_GLOBAL_STATE_NETWORK_OFF);

                ESP_LOGE(TAG, "event lost ip");
                break;

            default:
                break;
        }
    }
}

void app_wifi_init(void)
{
    bsp_register_callback(app_wifi_handler);
}