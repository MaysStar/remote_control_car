#include "app_ota_update.h"
#include "app_global_state.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

TickType_t test_tick;
static bool inhibited, acknowledged, complete, init_failed, partition_missing;
static int open_result, begin_result, write_result, end_result, boot_result;
static int init_calls, begin_calls, read_calls, abort_calls, end_calls, boot_calls, cleanup_calls;
static int status_code, read_result;
static int64_t clock_us, read_delay_us, image_size;
static int handle_token;
static const esp_partition_t partition = {.size = 1024 * 1024};

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

esp_http_client_handle_t esp_http_client_init(const esp_http_client_config_t *config)
{
    assert(inhibited && acknowledged);
    assert(config->disable_auto_redirect);
    assert(!config->skip_cert_common_name_check);
    assert(config->crt_bundle_attach == esp_crt_bundle_attach);
    ++init_calls;
    return init_failed ? NULL : &handle_token;
}
esp_err_t esp_http_client_open(esp_http_client_handle_t client, int length)
{
    assert(client == &handle_token && length == 0);
    return open_result;
}
int64_t esp_http_client_fetch_headers(esp_http_client_handle_t client)
{
    assert(client == &handle_token);
    return image_size;
}
int esp_http_client_get_status_code(esp_http_client_handle_t client)
{
    assert(client == &handle_token);
    return status_code;
}
int esp_http_client_read(esp_http_client_handle_t client, char *buffer, int length)
{
    assert(client == &handle_token);
    ++read_calls;
    clock_us += read_delay_us;
    if (read_result <= 0)
    {
        return read_result;
    }
    const int count = read_result < length ? read_result : length;
    memset(buffer, 0, (size_t)count);
    return count;
}
bool esp_http_client_is_complete_data_received(esp_http_client_handle_t client)
{
    assert(client == &handle_token);
    return complete;
}
esp_err_t esp_http_client_close(esp_http_client_handle_t client)
{
    assert(client == &handle_token);
    return ESP_OK;
}
esp_err_t esp_http_client_cleanup(esp_http_client_handle_t client)
{
    assert(client == &handle_token);
    ++cleanup_calls;
    return ESP_OK;
}
const esp_partition_t *esp_ota_get_next_update_partition(const esp_partition_t *current)
{
    assert(current == NULL);
    return partition_missing ? NULL : &partition;
}
esp_err_t esp_ota_begin(const esp_partition_t *target, size_t size, esp_ota_handle_t *update)
{
    assert(target == &partition && size == (size_t)image_size);
    ++begin_calls;
    *update = 42;
    return begin_result;
}
esp_err_t esp_ota_write(esp_ota_handle_t update, const void *buffer, size_t size)
{
    assert(update == 42 && buffer != NULL && size > 0);
    return write_result;
}
esp_err_t esp_ota_end(esp_ota_handle_t update)
{
    assert(update == 42);
    ++end_calls;
    return end_result;
}
esp_err_t esp_ota_abort(esp_ota_handle_t update)
{
    assert(update == 42);
    ++abort_calls;
    return ESP_OK;
}
esp_err_t esp_ota_set_boot_partition(const esp_partition_t *target)
{
    assert(target == &partition && end_calls == 1 && end_result == ESP_OK);
    ++boot_calls;
    return boot_result;
}

static void reset_case(void)
{
    test_tick = 0;
    clock_us = read_delay_us = 0;
    inhibited = init_failed = partition_missing = false;
    acknowledged = complete = true;
    open_result = begin_result = write_result = end_result = boot_result = ESP_OK;
    init_calls = begin_calls = read_calls = abort_calls = end_calls = boot_calls = cleanup_calls =
        0;
    status_code = 200;
    image_size = 16385;
    read_result = 8192;
}
static void expect_result(int expected)
{
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == expected);
    assert(inhibited);
    assert(cleanup_calls == (init_failed ? 0 : 1));
}
int main(void)
{
    reset_case();
    acknowledged = false;
    test_tick = UINT32_MAX - 100;
    assert(app_ota_run_update(HTTPS_OTA_UPDATE_IP_ADDR) == ESP_ERR_TIMEOUT);
    assert(inhibited && init_calls == 0);
    const char *invalid_urls[] = {NULL, "", "http://host/fw", "https://", "https:///fw"};
    for (size_t i = 0; i < sizeof(invalid_urls) / sizeof(invalid_urls[0]); ++i)
    {
        reset_case();
        assert(app_ota_run_update(invalid_urls[i]) == ESP_ERR_INVALID_ARG);
        assert(inhibited && init_calls == 0);
    }
    reset_case();
    init_failed = true;
    expect_result(ESP_ERR_NO_MEM);
    reset_case();
    open_result = 99;
    expect_result(99);
    assert(begin_calls == 0);

    const int invalid_status[] = {301, 302, 303, 307, 308, 206, 401, 404, 500};
    for (size_t i = 0; i < sizeof(invalid_status) / sizeof(invalid_status[0]); ++i)
    {
        reset_case();
        status_code = invalid_status[i];
        expect_result(ESP_ERR_INVALID_SIZE);
        assert(begin_calls == 0 && read_calls == 0 && boot_calls == 0);
    }
    const int64_t invalid_sizes[] = {-1, 0, 1024 * 1024 + 1};
    for (size_t i = 0; i < sizeof(invalid_sizes) / sizeof(invalid_sizes[0]); ++i)
    {
        reset_case();
        image_size = invalid_sizes[i];
        expect_result(ESP_ERR_INVALID_SIZE);
        assert(begin_calls == 0);
    }
    reset_case();
    partition_missing = true;
    expect_result(ESP_ERR_INVALID_SIZE);
    reset_case();
    begin_result = 98;
    expect_result(98);
    assert(abort_calls == 0 && read_calls == 0);

    for (int count = -1; count <= 0; ++count)
    {
        reset_case();
        read_result = count;
        expect_result(ESP_FAIL);
        assert(abort_calls == 1 && end_calls == 0 && boot_calls == 0);
    }
    reset_case();
    write_result = 97;
    expect_result(97);
    assert(abort_calls == 1 && boot_calls == 0);
    reset_case();
    complete = false;
    expect_result(ESP_ERR_INVALID_STATE);
    assert(abort_calls == 1 && end_calls == 0);
    reset_case();
    read_delay_us = 120LL * 1000 * 1000;
    expect_result(ESP_ERR_TIMEOUT);
    assert(abort_calls == 1 && end_calls == 0);
    reset_case();
    end_result = 96;
    expect_result(96);
    assert(abort_calls == 0 && end_calls == 1 && boot_calls == 0);
    reset_case();
    boot_result = 95;
    expect_result(95);
    assert(abort_calls == 0 && boot_calls == 1);
    reset_case();
    expect_result(ESP_OK);
    assert(begin_calls == 1 && read_calls == 3 && end_calls == 1 && boot_calls == 1 &&
           abort_calls == 0);
    puts("OTA transport, failure and completion tests passed");
}
