#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
typedef uint32_t TickType_t;
#define configASSERT assert
#define portMAX_DELAY UINT32_MAX
#define pdMS_TO_TICKS(x) (x)
