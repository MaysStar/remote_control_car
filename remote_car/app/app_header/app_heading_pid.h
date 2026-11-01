#ifndef APP_HEADING_PID_H
#define APP_HEADING_PID_H

#include <stdbool.h>

/* Shared with the task schedule: gains below assume one sample every 50 ms. */
#define APP_HEADING_PID_PERIOD_MS 50U
#define APP_HEADING_PID_OUTPUT_LIMIT 100.0f

typedef struct
{
    float previous_error;
    float previous_measurement;
    float integral;
    float derivative;
    bool has_measurement;
} app_heading_pid;

/* Reset when stopping or changing direction. No hardware or RTOS dependencies. */
void app_heading_pid_reset(app_heading_pid *controller);

/* Degrees. Invalid inputs/state reset the controller, set output to zero and return false.
   The first sample initializes derivative history without a measurement kick. */
bool app_heading_pid_update(app_heading_pid *controller, float measurement_degrees,
                            float target_degrees, float *output);

#endif
