#include "app_car_motor_control.h"
#include "app_global_state.h"
#include "app_heading_pid.h"
#include "freertos/task.h"
#include "osal.h"

#define APP_MOTOR_HEADING_CORRECTION_DIVISOR 2.0f
#define APP_MOTOR_ACCELERATION_STEP_PERCENT 5U

_Static_assert(APP_MOTOR_ACCELERATION_STEP_PERCENT > 0U &&
                   APP_MOTOR_ACCELERATION_STEP_PERCENT <= APP_COMMAND_MAX_SPEED,
               "Motor acceleration step must fit the PWM range");

static uint32_t app_motor_ramp_speed(uint32_t current_speed, uint32_t target_speed)
{
    if (target_speed > current_speed &&
        target_speed - current_speed > APP_MOTOR_ACCELERATION_STEP_PERCENT)
    {
        return current_speed + APP_MOTOR_ACCELERATION_STEP_PERCENT;
    }
    return target_speed;
}

static uint32_t app_motor_corrected_speed(uint8_t requested_speed, float correction)
{
    int32_t speed = requested_speed + (int32_t)(correction / APP_MOTOR_HEADING_CORRECTION_DIVISOR);
    if (speed < 0)
    {
        return 0U;
    }
    if (speed > (int32_t)APP_COMMAND_MAX_SPEED)
    {
        return APP_COMMAND_MAX_SPEED;
    }
    return (uint32_t)speed;
}

void app_car_motor_control_task(void *parameters)
{
    (void)parameters;
    const TickType_t period = pdMS_TO_TICKS(APP_HEADING_PID_PERIOD_MS);
    TickType_t last_wake = xTaskGetTickCount();
    app_heading_pid controller = {0};
    app_car_dir_state previous_direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP;
    float target_heading = 0.0f;
    uint32_t applied_speed = 0U;

    while (1)
    {
        if (app_is_motion_inhibited())
        {
            if (osal_stop_motors() == ESP_OK)
            {
                app_confirm_motor_stopped();
            }
            applied_speed = 0U;
            app_heading_pid_reset(&controller);
            previous_direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP;
            vTaskDelayUntil(&last_wake, period);
            continue;
        }

        app_car_dir_speed command = app_get_dir_speed();
        app_car_position position = {0};
        const bool position_valid = app_get_valid_position(&position);
        if (command.speed == 0 || command.speed > APP_COMMAND_MAX_SPEED || !position_valid)
        {
            command.direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP;
        }
        /* A moving direction change first requests one complete zero-PWM cycle.
           Retry a failed stop before allowing a new direction. */
        if (command.direction != previous_direction &&
            previous_direction != APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP)
        {
            if (osal_stop_motors() == ESP_OK)
            {
                applied_speed = 0U;
                previous_direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP;
            }
            app_heading_pid_reset(&controller);
            last_wake = xTaskGetTickCount();
            vTaskDelayUntil(&last_wake, period);
            continue;
        }

        if (command.direction != previous_direction)
        {
            app_heading_pid_reset(&controller);
            target_heading = position.yaw;
        }

        switch (command.direction)
        {
            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD:
            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_BACKWARD:
            {
                float correction = 0.0f;
                if (!app_heading_pid_update(&controller, position.yaw, target_heading, &correction))
                {
                    osal_stop_motors();
                    applied_speed = 0U;
                    command.direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP;
                    break;
                }
                const uint32_t target_speed = app_motor_corrected_speed(command.speed, correction);
                applied_speed = app_motor_ramp_speed(applied_speed, target_speed);
                if (command.direction == APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD)
                {
                    osal_mcpwm_forward(applied_speed);
                }
                else
                {
                    osal_mcpwm_backward(applied_speed);
                }
                break;
            }
            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_RIGHT:
                target_heading = position.yaw;
                applied_speed = app_motor_ramp_speed(applied_speed, command.speed);
                osal_mcpwm_right(applied_speed);
                break;

            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_LEFT:
                target_heading = position.yaw;
                applied_speed = app_motor_ramp_speed(applied_speed, command.speed);
                osal_mcpwm_left(applied_speed);
                break;

            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP:
            default:
                applied_speed = 0U;
                osal_stop_motors();
                app_heading_pid_reset(&controller);
                target_heading = position.yaw;
                break;
        }

        previous_direction = command.direction;
        vTaskDelayUntil(&last_wake, period);
    }
}
