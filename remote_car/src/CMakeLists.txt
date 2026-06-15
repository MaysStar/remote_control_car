# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources 
    "${CMAKE_SOURCE_DIR}/src/*.*"
    "${CMAKE_SOURCE_DIR}/bsp/bsp_src/*.*"
    "${CMAKE_SOURCE_DIR}/osal/osal_src/*.*"
    "${CMAKE_SOURCE_DIR}/app/app_src/*.*"
)

idf_component_register( SRCS    ${app_sources}
                        INCLUDE_DIRS "../bsp/bsp_header"
                                     "../osal/osal_header"
                                     "../app/app_header"

                        PRIV_REQUIRES nvs_flash esp_driver_gpio esp_wifi lwip esp_https_ota mbedtls
                        )

