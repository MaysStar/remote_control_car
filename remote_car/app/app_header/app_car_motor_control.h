#ifndef APP_CAR_MOTOR_CONTROL_H
#define APP_CAR_MOTOR_CONTROL_H

#include "osal.h"

#include "app_global_state.h"

#define CLAMP(x, min, max)  ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

typedef struct
{
    /* Controller constant */
    float Kp;
    float Ki;
    float Kd;

    /* Controller memory */
    float prev_error;
    float prev_measurement;

    /* 10 * dt */
    float tau;

    /* Calculated value */
    float proportional;
    float integrator;
    float differentiator;

    /* Limitation */
    float min_result;
    float max_result;

    /* Result */
    float out;
}PID_Controller_t;

void app_car_motor_control_task(void* pvParameters);

#endif /* APP_CAR_MOTOR_CONTROL_H */