#include "app_car_motor_control.h"
#include "app_global_state.h"
#include "app_heading_pid.h"
#include "freertos/task.h"
#include "osal.h"

#define APP_MOTOR_HEADING_CORRECTION_DIVISOR 2.0f

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

    while (1)
    {
        if (app_is_motion_inhibited())
        {
            if (osal_stop_motors() == ESP_OK)
            {
                app_confirm_motor_stopped();
            }
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
                    command.direction = APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP;
                    break;
                }
                const uint32_t speed = app_motor_corrected_speed(command.speed, correction);
                if (command.direction == APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD)
                {
                    osal_mcpwm_forward(speed);
                }
                else
                {
                    osal_mcpwm_backward(speed);
                }
                break;
            }
            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_RIGHT:
                target_heading = position.yaw;
                osal_mcpwm_right(command.speed);
                break;

            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_LEFT:
                target_heading = position.yaw;
                osal_mcpwm_left(command.speed);
                break;

            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP:
            default:
                osal_stop_motors();
                app_heading_pid_reset(&controller);
                target_heading = position.yaw;
                break;
        }

        previous_direction = command.direction;
        vTaskDelayUntil(&last_wake, period);
    }
}
