#include "app_udp_log.h"

static volatile int32_t sock = -1;
static struct sockaddr_in dect_addr;
static const char* TAG = "APP_LOGS";

/* Task for sending logs in local server via UDP */
void app_udp_log_task(void* pvParameters)
{
    /* Create socket */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) 
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    }

    /* Configure destination address, family and ip_protocol */
    dect_addr.sin_addr.s_addr = inet_addr(LOGS_HOST_IP_ADDR);
    dect_addr.sin_family = AF_INET;
    dect_addr.sin_port = htons(LOGS_HOST_PORT_ADDR);

    while(1)
    {
        if((bsp_get_new_logs_number() > 0) && (app_get_network_flag() == APP_GLOBAL_STATE_NETWORK_ON))
        {
            /* Get log */
            char log_buf[BSP_STACK_LOG_SIZE];
            bsp_get_new_log(log_buf, BSP_STACK_LOG_SIZE);

            /* Send log into UDP server */
            int err = sendto(sock, log_buf, strlen(log_buf), 0, (struct sockaddr *)&dect_addr, sizeof(dect_addr));

            if (err < 0) 
            {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(150));
    }
}