#include "app_ota_update.h"
#include "app_global_state.h"
#include <assert.h>
#include <stdio.h>

TickType_t test_tick;
static bool inhibited;
static bool acknowledged;
static bool complete;
static int begin_result;
static int perform_result;
static int finish_result;
static int begin_calls;
static int perform_calls;
static int abort_calls;
static int finish_calls;
static int64_t clock_us;
static int handle_token;

void app_request_motion_inhibit(void)
{
    inhibited = true;
}
bool app_is_motor_stop_confirmed(void)
{
    return acknowledged;
}
void vTaskDelay(TickType_t ticks)
{
    test_tick += ticks;
    clock_us += (int64_t)ticks * 1000;
}
int64_t esp_timer_get_time(void)
{
    return clock_us;
}
int esp_crt_bundle_attach(void *config)
{
    (void)config;
    return ESP_OK;
}
int osal_get_ota_state(void)
{
    return 0;
}
void esp_restart(void)
{
    assert(false);
}

esp_err_t esp_https_ota_begin(const esp_https_ota_config_t *config, esp_https_ota_handle_t *handle)
{
    assert(inhibited && acknowledged);
    assert(config->http_config->disable_auto_redirect);
    assert(!config->http_config->skip_cert_common_name_check);
    assert(config->http_config->crt_bundle_attach == esp_crt_bundle_attach);
    ++begin_calls;
    if (begin_result == ESP_OK)
    {
        *handle = &handle_token;
    }
    return begin_result;
}
esp_err_t esp_https_ota_perform(esp_https_ota_handle_t handle)
{
    assert(handle == &handle_token);
    ++perform_calls;
    return perform_result;
}
bool esp_https_ota_is_complete_data_received(esp_https_ota_handle_t handle)
{
    assert(handle == &handle_token);
    return complete;
}
esp_err_t esp_https_ota_finish(esp_https_ota_handle_t handle)
{
    assert(handle == &handle_token);
    ++finish_calls;
    return finish_result;
}
esp_err_t esp_https_ota_abort(esp_https_ota_handle_t handle)
{
    assert(handle == &handle_token);
    ++abort_calls;
    return ESP_OK;
}

static void reset_case(void)
{
    test_tick = 0;
    clock_us = 0;
    inhibited = false;
    acknowledged = true;
    complete = true;
    begin_result = ESP_OK;
    perform_result = ESP_OK;
    finish_result = ESP_OK;
    begin_calls = perform_calls = abort_calls = finish_calls = 0;
}

int main(void)
{
    reset_case();
    acknowledged = false;
    test_tick = UINT32_MAX - 100;
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == ESP_ERR_TIMEOUT);
    assert(inhibited && begin_calls == 0);

    const char *invalid_urls[] = {NULL, "", "http://host/fw", "https://", "https:///fw"};
    for (size_t i = 0; i < sizeof(invalid_urls) / sizeof(invalid_urls[0]); ++i)
    {
        reset_case();
        assert(app_ota_run_update(invalid_urls[i]) == ESP_ERR_INVALID_ARG);
        assert(inhibited && begin_calls == 0);
    }

    reset_case();
    begin_result = 99;
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == 99);
    assert(begin_calls == 1 && perform_calls == 0 && abort_calls == 0 && finish_calls == 0);

    reset_case();
    perform_result = 98;
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == 98);
    assert(abort_calls == 1 && finish_calls == 0);

    reset_case();
    complete = false;
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == ESP_ERR_INVALID_STATE);
    assert(abort_calls == 1 && finish_calls == 0);

    reset_case();
    perform_result = ESP_ERR_HTTPS_OTA_IN_PROGRESS;
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == ESP_ERR_TIMEOUT);
    assert(abort_calls == 1 && finish_calls == 0 && perform_calls > 1);

    reset_case();
    finish_result = 97;
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == 97);
    assert(abort_calls == 0 && finish_calls == 1);

    reset_case();
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == ESP_OK);
    assert(inhibited && begin_calls == 1 && perform_calls == 1 && finish_calls == 1 &&
           abort_calls == 0);
    puts("OTA failure and completion tests passed");
}
