#pragma once
typedef void *SemaphoreHandle_t;
static inline void *xSemaphoreCreateMutex(void) { return (void*)1; }
static inline int xSemaphoreTake(void *m, unsigned t) { (void)m; (void)t; return 1; }
static inline int xSemaphoreGive(void *m) { (void)m; return 1; }
