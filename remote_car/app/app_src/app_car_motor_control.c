#include "app_car_motor_control.h"

/* Private variables and functions */
static PID_Controller_t PID_Controller = {0};
static float curr_target_angle = 0.0f;

static void app_motor_control_pid_controller_init(PID_Controller_t* PID_Controller);
static float app_motor_control_pid_update(PID_Controller_t* PID_controler, float measurement, float setpoint);

static void app_motor_control_pid_controller_init(PID_Controller_t* PID_Controller)
{
	if(PID_Controller != NULL)
	{
		PID_Controller->Kp = 4.0f;
		PID_Controller->Ki = 1.5f;
		PID_Controller->Kd = 0.02f;

		PID_Controller->min_result = -100.0f;
		PID_Controller->max_result = 100.0f;

		PID_Controller->prev_error = 0.0f;
		PID_Controller->prev_measurement = 0.0f;

		PID_Controller->proportional = 0.0f;
		PID_Controller->integrator = 0.0f;
		PID_Controller->differentiator = 0.0f;

		/* 10 * dt, dt = 50ms tau = 500ms */
		PID_Controller->tau = 0.05f;

		PID_Controller->out = 0.0f;
	}
}

/* Fuction which control the speed of the motors speed, using Proportional Integration Differential
    u(t) = Kp * e(t) + Ki * iteg(e(t)dt) + Kd * de(t) / dt
*/
static float app_motor_control_pid_update(PID_Controller_t* PID_controler, float measurement, float setpoint)
{
	/* Calculate error */
	float curr_error = setpoint - measurement;

    if(curr_error > 180.0f)
    {
        curr_error += -360.0f;
    }

    if(curr_error < -180.0f)
    {
        curr_error += 360.0f;
    }

	float dt_f = 0.05f;

	/* Proportional */
	PID_controler->proportional = PID_controler->Kp * curr_error;

	/* Integrator Trapezoidal Rule */
	PID_controler->integrator = PID_controler->integrator + 0.5f * PID_controler->Ki * (PID_controler->prev_error + curr_error) * dt_f;

	/* Band-Limited Derivative */
	float diff_num = measurement - PID_controler->prev_measurement;
	PID_controler->differentiator =   (2.0f * PID_controler->Kd * diff_num
	                      - (2.0f * PID_controler->tau - dt_f) * PID_controler->differentiator)
	                      / (2.0f * PID_controler->tau + dt_f);
	/* Set prev measurement */
	PID_controler->prev_error = curr_error;
	PID_controler->prev_measurement = measurement;

	/* Calculate output */
	float out_raw = PID_controler->proportional + PID_controler->integrator - PID_controler->differentiator;

	/* Anti-windup back-calculation */
	float out_clamped = CLAMP(out_raw, PID_controler->min_result, PID_controler->max_result);
	PID_controler->integrator += (out_clamped - out_raw) * 0.1f;

	/* Assign final output */
	PID_controler->out = out_clamped;

	return PID_controler->out;
}

void app_car_motor_control_task(void* pvParameters)
{
    /* Use vTaskDelayUntil to make stable dt for PID controller */
    const TickType_t xFrequency = pdMS_TO_TICKS(50);

    // Initialise the xLastWakeTime variable with the current time.
    TickType_t xLastWakeTime = xTaskGetTickCount();

    app_motor_control_pid_controller_init(&PID_Controller);

    while(1)
    {
        app_car_dir_speed curr_car_dir_speed = app_get_dir_speed();
        app_car_position curr_car_position = app_get_position();

        switch(curr_car_dir_speed.direction)
        {
            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_FORWARD:
            {
                // Calculate angle error and set correcting offset
                float curr_offset = app_motor_control_pid_update(&PID_Controller, curr_car_position.yaw, curr_target_angle);

                int32_t left_speed = curr_car_dir_speed.speed - (int32_t)(curr_offset / 2.0f);
                int32_t right_speed = curr_car_dir_speed.speed + (int32_t)(curr_offset / 2.0f);

                // Set limitation
                left_speed = (left_speed < 0) ? 0 : left_speed;
                left_speed = (left_speed > 90) ? 90 : left_speed;

                right_speed = (right_speed < 0) ? 0 : right_speed;
                right_speed = (right_speed > 90) ? 90 : right_speed;

                osal_mcpwm_forward(left_speed, right_speed);
                
                break;
            }

            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_BACKWARD:
            {
                // Calculate angle error and set correcting offset
                float curr_offset = app_motor_control_pid_update(&PID_Controller, curr_car_position.yaw, curr_target_angle);

                int32_t left_speed = curr_car_dir_speed.speed + (int32_t)(curr_offset / 2.0f);
                int32_t right_speed = curr_car_dir_speed.speed - (int32_t)(curr_offset / 2.0f);

                // Set limitation 
                left_speed = (left_speed < 0) ? 0 : left_speed;
                left_speed = (left_speed > 90) ? 90 : left_speed;

                right_speed = (right_speed < 0) ? 0 : right_speed;
                right_speed = (right_speed > 90) ? 90 : right_speed;

                osal_mcpwm_backward(left_speed, right_speed);
                
                break;
            }

            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_RIGHT:
            {
                curr_target_angle = curr_car_position.yaw;

                osal_mcpwm_right(curr_car_dir_speed.speed);

                break;
            }

            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_LEFT:
            {
                curr_target_angle = curr_car_position.yaw;

                osal_mcpwm_left(curr_car_dir_speed.speed);

                break;
            }

            case APP_MQTT_COMMUNICATION_CAR_DIRECTION_STOP:
            {
                curr_target_angle = curr_car_position.yaw;

                osal_mcpwm_forward(0, 0);

                break;
            }

            default:
                break;
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}