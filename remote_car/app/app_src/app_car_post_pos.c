#include "app_car_post_pos.h"

void app_car_http_post_position_task(void* pvParameters)
{
    while(1)
    {
        /* Get current position and POST in server */
        app_car_position curr_pos = app_get_position();

        osal_http_post_pos(curr_pos.roll, curr_pos.pitch, curr_pos.yaw);

        vTaskDelay(pdTICKS_TO_MS(100));
    }

    /* This line will never be performed, but better to leave it here */
    osal_http_cleanup();
}