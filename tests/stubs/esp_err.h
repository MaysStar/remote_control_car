#ifndef TEST_ESP_ERR_H
#define TEST_ESP_ERR_H
#include <assert.h>

typedef int esp_err_t;
#define ESP_OK 0
#define ESP_ERROR_CHECK(result) assert((result) == ESP_OK)
#endif
