#pragma once
#include "FreeRTOS.h"
extern TickType_t test_tick;
static inline TickType_t xTaskGetTickCount(void)
{
    return test_tick;
}

void vTaskDelayUntil(TickType_t *last_wake, TickType_t period);
