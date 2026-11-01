#include "app_heading_pid.h"
#include <math.h>
#include <stddef.h>

#define APP_HEADING_PID_PROPORTIONAL_GAIN 4.0f
#define APP_HEADING_PID_INTEGRAL_GAIN 1.5f
#define APP_HEADING_PID_DERIVATIVE_GAIN 0.02f
#define APP_HEADING_PID_FILTER_SECONDS 0.05f
#define APP_HEADING_PID_ANTI_WINDUP_GAIN 0.1f

static float app_heading_wrap_degrees(float angle)
{
    float wrapped = fmodf(angle, 360.0f);
    if (wrapped > 180.0f)
    {
        wrapped -= 360.0f;
    }
    else if (wrapped < -180.0f)
    {
        wrapped += 360.0f;
    }
    return wrapped;
}

static float app_heading_limit_output(float output)
{
    if (output > APP_HEADING_PID_OUTPUT_LIMIT)
    {
        return APP_HEADING_PID_OUTPUT_LIMIT;
    }
    if (output < -APP_HEADING_PID_OUTPUT_LIMIT)
    {
        return -APP_HEADING_PID_OUTPUT_LIMIT;
    }
    return output;
}

void app_heading_pid_reset(app_heading_pid *controller)
{
    if (controller != NULL)
    {
        *controller = (app_heading_pid){0};
    }
}

bool app_heading_pid_update(app_heading_pid *controller, float measurement_degrees,
                            float target_degrees, float *output)
{
    if (output != NULL)
    {
        *output = 0.0f;
    }
    if (controller == NULL || output == NULL || !isfinite(measurement_degrees) ||
        !isfinite(target_degrees))
    {
        app_heading_pid_reset(controller);
        return false;
    }
    if (!isfinite(controller->previous_error) || !isfinite(controller->previous_measurement) ||
        !isfinite(controller->integral) || !isfinite(controller->derivative))
    {
        app_heading_pid_reset(controller);
        return false;
    }

    /* Normalize before subtraction so large finite inputs cannot overflow. */
    const float measurement = app_heading_wrap_degrees(measurement_degrees);
    const float target = app_heading_wrap_degrees(target_degrees);
    const float error = app_heading_wrap_degrees(target - measurement);
    const float period_seconds = APP_HEADING_PID_PERIOD_MS / 1000.0f;
    const float measurement_delta =
        controller->has_measurement
            ? app_heading_wrap_degrees(measurement - controller->previous_measurement)
            : 0.0f;
    const float derivative =
        (2.0f * APP_HEADING_PID_DERIVATIVE_GAIN * measurement_delta -
         (2.0f * APP_HEADING_PID_FILTER_SECONDS - period_seconds) * controller->derivative) /
        (2.0f * APP_HEADING_PID_FILTER_SECONDS + period_seconds);
    float integral = controller->integral + 0.5f * APP_HEADING_PID_INTEGRAL_GAIN *
                                                (controller->previous_error + error) *
                                                period_seconds;
    const float raw_output = APP_HEADING_PID_PROPORTIONAL_GAIN * error + integral - derivative;
    const float limited_output = app_heading_limit_output(raw_output);
    integral += (limited_output - raw_output) * APP_HEADING_PID_ANTI_WINDUP_GAIN;

    if (!isfinite(derivative) || !isfinite(integral) || !isfinite(raw_output))
    {
        app_heading_pid_reset(controller);
        return false;
    }

    controller->previous_error = error;
    controller->previous_measurement = measurement;
    controller->integral = integral;
    controller->derivative = derivative;
    controller->has_measurement = true;
    *output = limited_output;
    return true;
}
