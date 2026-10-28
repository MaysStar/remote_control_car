
#include "bsp_mcpwm.h"

/* Private variables and functions */
static const char* TAG = "BSP_MCPWM";

typedef enum
{
    MOTOR_A_LEFT = 0,
    MOTOR_A_RIGHT,
    MOTOR_B_LEFT,
    MOTOR_B_RIGHT,
    MOTOR_COUNT
} bsp_motor_t;

static mcpwm_timer_handle_t mcpwm_timer_handle = NULL;

/* Two operators, one for each motor phase */
static mcpwm_oper_handle_t mcpwm_oper_a_handle = NULL;
static mcpwm_oper_handle_t mcpwm_oper_b_handle = NULL;

/* Two comparators for four PWM generators */
static mcpwm_cmpr_handle_t mcpwm_cmpr_a_handle = NULL;
static mcpwm_cmpr_handle_t mcpwm_cmpr_b_handle = NULL;

/* Four PWM generators */
static mcpwm_gen_handle_t mcpwm_gen_handles[MOTOR_COUNT] = {NULL};

static const gpio_num_t mcpwm_pwm_pins[MOTOR_COUNT] =
{
    BSP_MCPWM_PHASE_A_LEFT_PWM,
    BSP_MCPWM_PHASE_A_RIGHT_PWM,
    BSP_MCPWM_PHASE_B_LEFT_PWM,
    BSP_MCPWM_PHASE_B_RIGHT_PWM
};

static const gpio_num_t mcpwm_forward_pins[MOTOR_COUNT] =
{
    BSP_MCPWM_PHASE_A_LEFT_FORWARD,
    BSP_MCPWM_PHASE_A_RIGHT_FORWARD,
    BSP_MCPWM_PHASE_B_LEFT_FORWARD,
    BSP_MCPWM_PHASE_B_RIGHT_FORWARD
};

static const gpio_num_t mcpwm_backward_pins[MOTOR_COUNT] =
{
    BSP_MCPWM_PHASE_A_LEFT_BACKWARD,
    BSP_MCPWM_PHASE_A_RIGHT_BACKWARD,
    BSP_MCPWM_PHASE_B_LEFT_BACKWARD,
    BSP_MCPWM_PHASE_B_RIGHT_BACKWARD
};

/* Set all PWM outputs to the requested forced level.
 *  0 = LOW
 *  1 = HIGH
 * -1 = release force and use normal PWM events
 */
static void bsp_mcpwm_force_pwm(int level)
{
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        ESP_ERROR_CHECK(mcpwm_generator_set_force_level(
            mcpwm_gen_handles[i], level, true));
    }
}

/* Set direction of a single motor */
static void bsp_mcpwm_set_motor_direction(
    bsp_motor_t motor,
    bool forward,
    bool backward)
{
    ESP_ERROR_CHECK(gpio_set_level(
        mcpwm_forward_pins[motor], forward));

    ESP_ERROR_CHECK(gpio_set_level(
        mcpwm_backward_pins[motor], backward));
}

/* Set PWM duty cycle for all four motors */
static void bsp_mcpwm_set_speed(uint32_t speed_percent)
{
    if (speed_percent > 100U)
    {
        speed_percent = 100U;
    }

    uint32_t compare_value =
        (uint32_t)((uint64_t)BSP_MCPWM_TIMER_PERIOD_TICKS *
                   speed_percent / 100U);

    /* Keep compare value below the timer period */
    if (compare_value >= BSP_MCPWM_TIMER_PERIOD_TICKS)
    {
        compare_value = BSP_MCPWM_TIMER_PERIOD_TICKS - 1U;
    }

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
        mcpwm_cmpr_a_handle, compare_value));

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
        mcpwm_cmpr_b_handle, compare_value));

    if (speed_percent == 0U)
    {
        bsp_mcpwm_force_pwm(0);
    }
    else if (speed_percent == 100U)
    {
        bsp_mcpwm_force_pwm(1);
    }
    else
    {
        bsp_mcpwm_force_pwm(-1);
    }
}

/* Initialize motor GPIO directions and all MCPWM resources */
void bsp_mcpwm_init(void)
{
    /* Direction GPIO configuration */
    gpio_config_t dir_gpio_cfg =
    {
        .pin_bit_mask =
            (1ULL << BSP_MCPWM_PHASE_A_LEFT_FORWARD)   |
            (1ULL << BSP_MCPWM_PHASE_A_LEFT_BACKWARD)  |
            (1ULL << BSP_MCPWM_PHASE_A_RIGHT_FORWARD)  |
            (1ULL << BSP_MCPWM_PHASE_A_RIGHT_BACKWARD) |
            (1ULL << BSP_MCPWM_PHASE_B_LEFT_FORWARD)   |
            (1ULL << BSP_MCPWM_PHASE_B_LEFT_BACKWARD)  |
            (1ULL << BSP_MCPWM_PHASE_B_RIGHT_FORWARD)  |
            (1ULL << BSP_MCPWM_PHASE_B_RIGHT_BACKWARD),

        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(gpio_config(&dir_gpio_cfg));

    /* All motors stopped initially */
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        bsp_mcpwm_set_motor_direction((bsp_motor_t)i, false, false);
    }

    /* 1. Configure and set new timer */
    mcpwm_timer_config_t mcpwm_timer_config =
    {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .flags.update_period_on_empty =
            BSP_MCPWM_TIMER_UPDATE_ON_EMPTY,
        .group_id = BSP_MCPWM_TIMER_GROUP_ID,
        .intr_priority = BSP_MCPWM_TIMER_INTR_PRIORITY,
        .period_ticks = BSP_MCPWM_TIMER_PERIOD_TICKS,
        .resolution_hz = BSP_MCPWM_TIMER_RESOLUTION_HZ
    };

    ESP_ERROR_CHECK(mcpwm_new_timer(
        &mcpwm_timer_config, &mcpwm_timer_handle));

    /* 2. Configure and set new operators */
    mcpwm_operator_config_t mcpwm_operator_config =
    {
        .flags.update_gen_action_on_tez =
            BSP_MCPWM_OPERATOR_UPDATE_ON_ZERO,
        .group_id = BSP_MCPWM_OPERATOR_GROUP_ID,
        .intr_priority = BSP_MCPWM_OPERATOR_INTR_PRIORITY
    };

    ESP_ERROR_CHECK(mcpwm_new_operator(
        &mcpwm_operator_config, &mcpwm_oper_a_handle));

    ESP_ERROR_CHECK(mcpwm_new_operator(
        &mcpwm_operator_config, &mcpwm_oper_b_handle));

    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(
        mcpwm_oper_a_handle, mcpwm_timer_handle));

    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(
        mcpwm_oper_b_handle, mcpwm_timer_handle));

    /* 3. Configure and set new comparators */
    mcpwm_comparator_config_t mcpwm_comparator_config =
    {
        .flags.update_cmp_on_tez =
            BSP_MCPWM_COMPARATOR_UPDATE_ON_ZERO,
        .intr_priority = BSP_MCPWM_COMPARATOR_INTR_PRIORITY
    };

    ESP_ERROR_CHECK(mcpwm_new_comparator(
        mcpwm_oper_a_handle,
        &mcpwm_comparator_config,
        &mcpwm_cmpr_a_handle));

    ESP_ERROR_CHECK(mcpwm_new_comparator(
        mcpwm_oper_b_handle,
        &mcpwm_comparator_config,
        &mcpwm_cmpr_b_handle));

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
        mcpwm_cmpr_a_handle, 0));

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
        mcpwm_cmpr_b_handle, 0));

    /* 4. Configure and set new generators */
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        mcpwm_generator_config_t mcpwm_generator_config =
        {
            .gen_gpio_num = mcpwm_pwm_pins[i]
        };

        mcpwm_oper_handle_t operator_handle =
            (i < 2) ? mcpwm_oper_a_handle : mcpwm_oper_b_handle;

        ESP_ERROR_CHECK(mcpwm_new_generator(
            operator_handle,
            &mcpwm_generator_config,
            &mcpwm_gen_handles[i]));
    }

    /* 5. Configure comparator and timer events
     *
     * Timer EMPTY -> HIGH
     * Compare match -> LOW
     * Timer FULL -> LOW
     */
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        mcpwm_cmpr_handle_t comparator_handle =
            (i < 2) ? mcpwm_cmpr_a_handle : mcpwm_cmpr_b_handle;

        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
            mcpwm_gen_handles[i],
            MCPWM_GEN_COMPARE_EVENT_ACTION(
                MCPWM_TIMER_DIRECTION_UP,
                comparator_handle,
                MCPWM_GEN_ACTION_LOW)));

        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
            mcpwm_gen_handles[i],
            MCPWM_GEN_TIMER_EVENT_ACTION(
                MCPWM_TIMER_DIRECTION_UP,
                MCPWM_TIMER_EVENT_EMPTY,
                MCPWM_GEN_ACTION_HIGH)));

        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
            mcpwm_gen_handles[i],
            MCPWM_GEN_TIMER_EVENT_ACTION(
                MCPWM_TIMER_DIRECTION_UP,
                MCPWM_TIMER_EVENT_FULL,
                MCPWM_GEN_ACTION_LOW)));
    }

    /* Keep all PWM outputs LOW before starting timer */
    bsp_mcpwm_force_pwm(0);

    /* 6. Enable timer */
    ESP_ERROR_CHECK(mcpwm_timer_enable(mcpwm_timer_handle));

    /* 7. Start timer */
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(
        mcpwm_timer_handle, MCPWM_TIMER_START_NO_STOP));

    /* Forced stop from drive */
    bsp_mcpwm_stop();

    ESP_LOGI(TAG, "Successful start: 4 motors, PWM 20 kHz");
}

/* Common driving function
 *  1 = forward
 * -1 = backward
 *  0 = stop
 */
static void bsp_mcpwm_drive(
    uint32_t speed_percent,
    int a_left,
    int a_right,
    int b_left,
    int b_right)
{
    if (speed_percent == 0U)
    {
        bsp_mcpwm_stop();
        return;
    }

    /* Disable PWM before changing direction */
    bsp_mcpwm_force_pwm(0);

    /* Clear previous direction states */
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        bsp_mcpwm_set_motor_direction((bsp_motor_t)i, false, false);
    }

    const int directions[MOTOR_COUNT] =
    {
        a_left,
        a_right,
        b_left,
        b_right
    };

    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        bsp_mcpwm_set_motor_direction(
            (bsp_motor_t)i,
            directions[i] > 0,
            directions[i] < 0);
    }

    /* Apply speed and enable PWM */
    bsp_mcpwm_set_speed(speed_percent);
}

/* Move all motors forward */
void bsp_mcpwm_forward(uint32_t speed_percent)
{
    bsp_mcpwm_drive(speed_percent, 1, 1, 1, 1);
}

/* Move all motors backward */
void bsp_mcpwm_backward(uint32_t speed_percent)
{
    bsp_mcpwm_drive(speed_percent, -1, -1, -1, -1);
}

/* Turn right: left motors forward, right motors backward */
void bsp_mcpwm_right(uint32_t speed_percent)
{
    bsp_mcpwm_drive(speed_percent, 1, -1, 1, -1);
}

/* Turn left: left motors backward, right motors forward */
void bsp_mcpwm_left(uint32_t speed_percent)
{
    bsp_mcpwm_drive(speed_percent, -1, 1, -1, 1);
}

/* Stop all motors */
void bsp_mcpwm_stop(void)
{
    /* Immediately disable all PWM outputs */
    bsp_mcpwm_force_pwm(0);

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
        mcpwm_cmpr_a_handle, 0));

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
        mcpwm_cmpr_b_handle, 0));

    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        bsp_mcpwm_set_motor_direction((bsp_motor_t)i, false, false);
    }
}
