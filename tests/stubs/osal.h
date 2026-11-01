#ifndef TEST_OSAL_H
#define TEST_OSAL_H
#include <stdint.h>
typedef int esp_err_t;
#define ESP_OK 0
esp_err_t osal_stop_motors(void);
void osal_mcpwm_forward(uint32_t speed);
void osal_mcpwm_backward(uint32_t speed);
void osal_mcpwm_right(uint32_t speed);
void osal_mcpwm_left(uint32_t speed);
#endif
