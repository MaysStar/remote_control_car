#ifndef BSP_LOG_HOOK_H
#define BSP_LOG_HOOK_H

#include "assert.h"

#include "esp_err.h"
#include "esp_log.h"

#define BSP_STACK_SIZE 50U
#define BSP_STACK_LOG_SIZE 100U

void bsp_log_hook_init(void);

uint32_t bsp_get_new_logs_number(void);
void bsp_get_new_log(char* log, uint32_t log_size);

#endif /* BSP_LOG_HOOK_H */