#include "bsp_sntp.h"

static const char* TAG = "SNTP_TIME";

static void set_new_time(struct timeval *tv)
{
    /* Time structures */
    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;

    /* Set local time from global */
    time(&now);

    /* Print current time */
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
}

void bsp_start_sntp_time(Contry_time_zone tm_zn)
{  
    /* Set time zone */
    const char* curr_tz = "CST-8";
    switch (tm_zn)
    {
        case UA:
            curr_tz = "EET-2EEST,M3.5.0/3,M10.5.0/4";
            break;
        
        default:
            break;
    }

    ESP_ERROR_CHECK(setenv("TZ", curr_tz, 1));
    tzset();

    /* Set server to update time */
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    config.sync_cb = set_new_time;
    ESP_ERROR_CHECK(esp_netif_sntp_init(&config));
}