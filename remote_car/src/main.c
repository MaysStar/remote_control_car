#include "stdint.h"

#include "bsp_nvs_flash.h"
#include "bsp_sntp.h"
#include "osal.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main()
{
    bsp_nvs_flash_start();

    //bsp_start_sntp_time(UA);
}
