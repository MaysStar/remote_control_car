#include "app_global_state.h"
#include <stdbool.h>
#include <math.h>
#include "freertos/task.h"

/* Private variables and functions */
static SemaphoreHandle_t network_mutex = NULL;
static SemaphoreHandle_t position_mutex = NULL;
static SemaphoreHandle_t command_mutex = NULL;

static uint8_t network_state = APP_GLOBAL_STATE_NETWORK_OFF;
static app_car_position car_position;
static TickType_t last_position_tick;
static bool position_valid = false;
static app_car_dir_speed car_dir_speed = {.speed = 0,
                                          .direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP};
static TickType_t last_direction_tick;
static bool direction_valid = false;
static TickType_t last_speed_tick;
static bool speed_valid = false;

/* Caller must hold command_mutex. */
static void app_clear_command_locked(void)
{
    car_dir_speed.speed = 0U;
    car_dir_speed.direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP;
    direction_valid = false;
    speed_valid = false;
}

/* Initialize all state and data */
void app_global_state_init(void)
{
    network_mutex = xSemaphoreCreateMutex();
    configASSERT(network_mutex != NULL);

    position_mutex = xSemaphoreCreateMutex();
    configASSERT(position_mutex != NULL);

    command_mutex = xSemaphoreCreateMutex();
    configASSERT(command_mutex != NULL);
}

void app_set_network_flag(uint8_t new_net_fl)
{
    if (network_mutex != NULL)
    {
        xSemaphoreTake(network_mutex, portMAX_DELAY);
        network_state = new_net_fl;
        xSemaphoreGive(network_mutex);
    }
}

uint8_t app_get_network_flag(void)
{
    if (network_mutex != NULL)
    {
        uint8_t temp_state;
        xSemaphoreTake(network_mutex, portMAX_DELAY);
        temp_state = network_state;
        xSemaphoreGive(network_mutex);
        return temp_state;
    }

    return 0;
}

void app_set_position(app_car_position *curr_pos)
{
    if (position_mutex != NULL && curr_pos != NULL)
    {
        xSemaphoreTake(position_mutex, portMAX_DELAY);
        position_valid =
            isfinite(curr_pos->roll) && isfinite(curr_pos->pitch) && isfinite(curr_pos->yaw);
        if (position_valid)
        {
            car_position = *curr_pos;
            last_position_tick = xTaskGetTickCount();
        }
        xSemaphoreGive(position_mutex);
    }
}

app_car_position app_get_position(void)
{
    app_car_position curr_pos = {0};
    if (position_mutex != NULL)
    {
        xSemaphoreTake(position_mutex, portMAX_DELAY);
        curr_pos = car_position;
        xSemaphoreGive(position_mutex);
    }

    return curr_pos;
}

void app_set_dir(app_car_dir_state curr_dir)
{
    if (curr_dir < APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD ||
        curr_dir > APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP)
    {
        app_stop_motion();
        return;
    }

    if (command_mutex != NULL)
    {
        xSemaphoreTake(command_mutex, portMAX_DELAY);
        car_dir_speed.direction = curr_dir;
        last_direction_tick = xTaskGetTickCount();
        direction_valid = true;
        xSemaphoreGive(command_mutex);
    }
}

void app_set_speed(uint8_t curr_speed)
{
    if (curr_speed > APP_COMMAND_MAX_SPEED)
    {
        app_stop_motion();
        return;
    }

    if (command_mutex != NULL)
    {
        xSemaphoreTake(command_mutex, portMAX_DELAY);
        car_dir_speed.speed = curr_speed;
        last_speed_tick = xTaskGetTickCount();
        speed_valid = true;
        xSemaphoreGive(command_mutex);
    }
}

app_car_dir_speed app_get_dir_speed(void)
{
    app_car_dir_speed curr_dir_speed = {.speed = 0,
                                        .direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP};

    if (command_mutex != NULL)
    {
        xSemaphoreTake(command_mutex, portMAX_DELAY);
        const TickType_t now = xTaskGetTickCount();
        const TickType_t timeout = pdMS_TO_TICKS(APP_COMMAND_TIMEOUT_MS);

        if (direction_valid && speed_valid && (TickType_t)(now - last_direction_tick) < timeout &&
            (TickType_t)(now - last_speed_tick) < timeout)
        {
            curr_dir_speed = car_dir_speed;
        }
        else
        {
            app_clear_command_locked();
        }
        xSemaphoreGive(command_mutex);
    }

    return curr_dir_speed;
}
void app_stop_motion(void)
{
    if (command_mutex != NULL)
    {
        xSemaphoreTake(command_mutex, portMAX_DELAY);
        app_clear_command_locked();
        xSemaphoreGive(command_mutex);
    }
}

bool app_get_valid_position(app_car_position *position)
{
    if (position == NULL || position_mutex == NULL)
    {
        return false;
    }

    xSemaphoreTake(position_mutex, portMAX_DELAY);
    const TickType_t age = (TickType_t)(xTaskGetTickCount() - last_position_tick);
    const bool valid = position_valid && age < pdMS_TO_TICKS(APP_POSITION_TIMEOUT_MS);

    if (valid)
    {
        *position = car_position;
    }

    xSemaphoreGive(position_mutex);
    return valid;
}
