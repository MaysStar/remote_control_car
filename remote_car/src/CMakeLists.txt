# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

# Explicit source manifest: new files require build review.
set(app_sources
    "${CMAKE_SOURCE_DIR}/src/main.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_http_pos.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_log_hook.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_mcpwm.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_mpu6050.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_mqtt.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_nvs_flash.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_ota_update.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_sntp.c"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/bsp_wifi.c"
    "${CMAKE_SOURCE_DIR}/osal/osal_src/osal.c"
    "${CMAKE_SOURCE_DIR}/app/app_src/app_car_motor_control.c"
    "${CMAKE_SOURCE_DIR}/app/app_src/app_car_position.c"
    "${CMAKE_SOURCE_DIR}/app/app_src/app_car_post_pos.c"
    "${CMAKE_SOURCE_DIR}/app/app_src/app_global_state.c"
    "${CMAKE_SOURCE_DIR}/app/app_src/app_mqtt_communication.c"
    "${CMAKE_SOURCE_DIR}/app/app_src/app_ota_update.c"
    "${CMAKE_SOURCE_DIR}/app/app_src/app_udp_log.c"
    "${CMAKE_SOURCE_DIR}/app/app_src/app_wifi.c"
)

idf_component_register( SRCS    ${app_sources}
                        INCLUDE_DIRS "../bsp/bsp_header"
                                     "../osal/osal_header"
                                     "../app/app_header"

                        PRIV_REQUIRES nvs_flash esp_driver_gpio esp_wifi lwip esp_https_ota mbedtls esp_http_client esp_driver_mcpwm
                        )

