#include "app_heading_pid.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

static void assert_close(float actual, float expected)
{
    assert(fabsf(actual - expected) < 0.001f);
}

int main(void)
{
    app_heading_pid controller = {0};
    float output = 12.0f;
    assert(!app_heading_pid_update(NULL, 0, 0, &output));
    assert(output == 0);
    assert(!app_heading_pid_update(&controller, 0, 0, NULL));
    app_heading_pid_reset(NULL);

    assert(app_heading_pid_update(&controller, 90, 90, &output));
    assert_close(output, 0);
    app_heading_pid_reset(&controller);
    assert(app_heading_pid_update(&controller, 0, 10, &output));
    assert_close(output, 40.375f);

    const float invalid[] = {NAN, INFINITY, -INFINITY};
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
    {
        assert(!app_heading_pid_update(&controller, invalid[i], 0, &output));
        assert(output == 0 && !controller.has_measurement);
        assert(!app_heading_pid_update(&controller, 0, invalid[i], &output));
        assert(output == 0);
    }
    controller.integral = NAN;
    assert(!app_heading_pid_update(&controller, 0, 0, &output));
    assert(controller.integral == 0 && output == 0);

    /* Crossing +/-180 degrees is the same small movement as crossing zero. */
    app_heading_pid boundary = {0};
    app_heading_pid reference = {0};
    float boundary_output, reference_output;
    assert(app_heading_pid_update(&boundary, 179, 179, &boundary_output));
    assert(app_heading_pid_update(&reference, -1, -1, &reference_output));
    assert(app_heading_pid_update(&boundary, -179, 179, &boundary_output));
    assert(app_heading_pid_update(&reference, 1, -1, &reference_output));
    assert_close(boundary_output, reference_output);

    app_heading_pid_reset(&controller);
    assert(app_heading_pid_update(&controller, 720, 730, &output));
    assert_close(output, 40.375f);
    app_heading_pid_reset(&controller);
    assert(app_heading_pid_update(&controller, FLT_MAX, -FLT_MAX, &output));
    assert(isfinite(output));

    for (int i = 0; i < 10000; ++i)
    {
        assert(app_heading_pid_update(&controller, 0, 170, &output));
        assert(isfinite(controller.integral));
        assert(fabsf(output) <= APP_HEADING_PID_OUTPUT_LIMIT);
    }
    app_heading_pid_reset(&controller);
    assert(app_heading_pid_update(&controller, 0, 0, &output));
    assert(output == 0);

    /* Corrupt but finite state must never turn overflow into an actuator value. */
    controller.previous_error = FLT_MAX;
    controller.integral = FLT_MAX;
    controller.derivative = -FLT_MAX;
    assert(!app_heading_pid_update(&controller, 0, 0, &output));
    assert(output == 0 && !controller.has_measurement);
    puts("heading PID tests passed");
}
