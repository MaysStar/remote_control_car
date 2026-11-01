#include "app_car_motor_control.h"
#include "app_global_state.h"
#include "app_heading_pid.h"
#include "osal.h"
#include <assert.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>

typedef struct
{
    app_car_dir_state direction;
    uint8_t speed;
    float yaw;
    bool position_valid;
    bool inhibited;
    int stop_result;
    app_car_dir_state expected_direction;
    uint32_t expected_speed;
    bool expected_ack;
} motor_case;

TickType_t test_tick;
static jmp_buf task_finished;
static const motor_case *cases;
static size_t case_count;
static size_t case_index;
static unsigned writes;
static bool acknowledged;

bool app_is_motion_inhibited(void)
{
    return cases[case_index].inhibited;
}
app_car_dir_speed app_get_dir_speed(void)
{
    return (app_car_dir_speed){.direction = cases[case_index].direction,
                               .speed = cases[case_index].speed};
}
bool app_get_valid_position(app_car_position *position)
{
    position->yaw = cases[case_index].yaw;
    return cases[case_index].position_valid;
}
void app_confirm_motor_stopped(void)
{
    assert(writes == 1 && cases[case_index].stop_result == ESP_OK);
    acknowledged = true;
}
static void expect_write(app_car_dir_state direction, uint32_t speed)
{
    assert(direction == cases[case_index].expected_direction);
    assert(speed == cases[case_index].expected_speed);
    assert(++writes == 1);
}
esp_err_t osal_stop_motors(void)
{
    expect_write(APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, 0);
    return cases[case_index].stop_result;
}
void osal_mcpwm_forward(uint32_t speed)
{
    expect_write(APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, speed);
}
void osal_mcpwm_backward(uint32_t speed)
{
    expect_write(APP_MQTT_COMMUNICATION_CAR_DIRECTION_BACKWARD, speed);
}
void osal_mcpwm_right(uint32_t speed)
{
    expect_write(APP_MQTT_COMMUNICATION_CAR_DIRECTION_RIGHT, speed);
}
void osal_mcpwm_left(uint32_t speed)
{
    expect_write(APP_MQTT_COMMUNICATION_CAR_DIRECTION_LEFT, speed);
}
void vTaskDelayUntil(TickType_t *last_wake, TickType_t period)
{
    assert(period == pdMS_TO_TICKS(APP_HEADING_PID_PERIOD_MS));
    assert(writes == 1);
    assert(acknowledged == cases[case_index].expected_ack);
    *last_wake += period;
    test_tick = *last_wake;
    writes = 0;
    acknowledged = false;
    if (++case_index == case_count)
    {
        longjmp(task_finished, 1);
    }
}
int main(void)
{
    const motor_case scenarios[] = {
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 70, 90, true, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 70, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 70, 80, true, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 90, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_BACKWARD, 60, 80, true, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_BACKWARD, 60, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 0, 80, true, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, 0, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 60, 0, false, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, 0, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 255, 0, true, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, 0, false},
        {(app_car_dir_state)99, 60, 0, true, false, 0, APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, 0,
         false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 60, NAN, true, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, 0, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_RIGHT, 40, 0, true, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_RIGHT, 40, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_LEFT, 40, 0, true, false, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_LEFT, 40, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 90, 0, true, true, 1,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, 0, false},
        {APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD, 90, 0, true, true, 0,
         APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP, 0, true},
    };
    cases = scenarios;
    case_count = sizeof(scenarios) / sizeof(scenarios[0]);
    if (setjmp(task_finished) == 0)
    {
        app_car_motor_control_task(NULL);
    }
    puts("motor task integration tests passed");
}
