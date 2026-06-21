#include "bsp_mcpwm.h"

/* Private variables and functions */
static const char* TAG = "BSP_MCPWM";

static mcpwm_timer_handle_t mcpwm_timer_handle = NULL;

static mcpwm_oper_handle_t mcpwm_oper_right_handle = NULL;
static mcpwm_oper_handle_t mcpwm_oper_left_handle = NULL;

static mcpwm_cmpr_handle_t mcpwm_cmpr_right_handle = NULL;
static mcpwm_cmpr_handle_t mcpwm_cmpr_left_handle = NULL;

static mcpwm_gen_handle_t mcpwm_gen_right_a_handle = NULL;
static mcpwm_gen_handle_t mcpwm_gen_right_b_handle = NULL;

static mcpwm_gen_handle_t mcpwm_gen_left_a_handle = NULL;
static mcpwm_gen_handle_t mcpwm_gen_left_b_handle = NULL;

void bsp_mcpwm_init(void)
{
    /* 1. Configure and set new timer */
    mcpwm_timer_config_t mcpwm_timer_config = 
    {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .flags.update_period_on_empty = BSP_MCPWM_TIMER_UPDATE_ON_EMPTY,
        .group_id = BSP_MCPWM_TIMER_GROUP_ID,
        .intr_priority = BSP_MCPWM_TIMER_INTR_PRIORITY,
        .period_ticks = BSP_MCPWM_TIMER_PERIOD_TICKS, // period ticks or auto reload value
        .resolution_hz = 20 * 1000 * 1000 // timer resolution 20MHz
    };

    ESP_ERROR_CHECK(mcpwm_new_timer(&mcpwm_timer_config, &mcpwm_timer_handle));

    /* 2. Configure and set new operators */
    mcpwm_operator_config_t mcpwm_operator_config = 
    {
        .flags.update_gen_action_on_tez = BSP_MCPWM_OPERATOR_UPDATE_ON_ZERO,
        .group_id = BSP_MCPWM_OPERATOR_GROUP_ID,
        .intr_priority = BSP_MCPWM_OPERATOR_INTR_PRIORITY,
    };

    ESP_ERROR_CHECK(mcpwm_new_operator(&mcpwm_operator_config, &mcpwm_oper_right_handle));
    ESP_ERROR_CHECK(mcpwm_new_operator(&mcpwm_operator_config, &mcpwm_oper_left_handle));

    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(mcpwm_oper_right_handle, mcpwm_timer_handle));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(mcpwm_oper_left_handle, mcpwm_timer_handle));

    /* 3. Configure and set new comparators */
    mcpwm_comparator_config_t mcpwm_comparator_config = 
    {
        .flags.update_cmp_on_tez = BSP_MCPWM_COMPARATOR_UPDATE_ON_ZERO,
        .intr_priority = BSP_MCPWM_COMPARATOR_INTR_PRIORITY,
    };

    ESP_ERROR_CHECK(mcpwm_new_comparator(mcpwm_oper_right_handle, &mcpwm_comparator_config, &mcpwm_cmpr_right_handle));
    ESP_ERROR_CHECK(mcpwm_new_comparator(mcpwm_oper_left_handle, &mcpwm_comparator_config, &mcpwm_cmpr_left_handle));

    /* 4. Configure and set new generators */
    mcpwm_generator_config_t mcpwm_generator_config_right_a = 
    {
        .gen_gpio_num = BSP_MCPWM_RIGHT_PIN_A,
    };

    mcpwm_generator_config_t mcpwm_generator_config_right_b = 
    {
        .gen_gpio_num = BSP_MCPWM_RIGHT_PIN_B,
    };

    mcpwm_generator_config_t mcpwm_generator_config_left_a = 
    {
        .gen_gpio_num = BSP_MCPWM_LEFT_PIN_A,
    };

    mcpwm_generator_config_t mcpwm_generator_config_left_b = 
    {
        .gen_gpio_num = BSP_MCPWM_LEFT_PIN_B,
    };

    ESP_ERROR_CHECK(mcpwm_new_generator(mcpwm_oper_right_handle, &mcpwm_generator_config_right_a, &mcpwm_gen_right_a_handle));
    ESP_ERROR_CHECK(mcpwm_new_generator(mcpwm_oper_right_handle, &mcpwm_generator_config_right_b, &mcpwm_gen_right_b_handle));
    
    ESP_ERROR_CHECK(mcpwm_new_generator(mcpwm_oper_left_handle, &mcpwm_generator_config_left_a, &mcpwm_gen_left_a_handle));
    ESP_ERROR_CHECK(mcpwm_new_generator(mcpwm_oper_left_handle, &mcpwm_generator_config_left_b, &mcpwm_gen_left_b_handle));

    /* Configure comparator and timer event 
     *
     * Comparator first high, then low
     * Timer Empty high, Full low 
     */
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(mcpwm_gen_right_a_handle,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwm_cmpr_right_handle, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(mcpwm_gen_right_b_handle,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwm_cmpr_right_handle, MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(mcpwm_gen_left_a_handle,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwm_cmpr_left_handle, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(mcpwm_gen_left_b_handle,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwm_cmpr_left_handle, MCPWM_GEN_ACTION_LOW)));            


    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(mcpwm_gen_right_a_handle,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(mcpwm_gen_right_a_handle,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(mcpwm_gen_right_b_handle,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(mcpwm_gen_right_b_handle,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(mcpwm_gen_left_a_handle,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(mcpwm_gen_left_a_handle,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(mcpwm_gen_left_b_handle,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(mcpwm_gen_left_b_handle,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));

    /* Enable timer */
    ESP_ERROR_CHECK(mcpwm_timer_enable(mcpwm_timer_handle));

    /* Start timer */
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(mcpwm_timer_handle, MCPWM_TIMER_START_NO_STOP));

    /* Forced stop from drive */
    bsp_mcpwm_forward(0, 0);


    ESP_LOGI(TAG, " successful start");
}


void bsp_mcpwm_forward(uint32_t left_percent, uint32_t right_percent)
{
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr_right_handle, (uint32_t)(BSP_MCPWM_TIMER_PERIOD_TICKS * right_percent / 100U)));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr_left_handle, (uint32_t)(BSP_MCPWM_TIMER_PERIOD_TICKS * left_percent / 100U)));

    mcpwm_generator_set_force_level(mcpwm_gen_right_a_handle, -1, true);
    mcpwm_generator_set_force_level(mcpwm_gen_right_b_handle, 0, true);

    mcpwm_generator_set_force_level(mcpwm_gen_left_a_handle, -1, true);
    mcpwm_generator_set_force_level(mcpwm_gen_left_b_handle, 0, true);
}

void bsp_mcpwm_backward(uint32_t left_percent, uint32_t right_percent)
{
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr_right_handle, (uint32_t)(BSP_MCPWM_TIMER_PERIOD_TICKS * right_percent / 100U)));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr_left_handle, (uint32_t)(BSP_MCPWM_TIMER_PERIOD_TICKS * left_percent / 100U)));

    mcpwm_generator_set_force_level(mcpwm_gen_right_a_handle, 0, true);
    mcpwm_generator_set_force_level(mcpwm_gen_right_b_handle, -1, true);

    mcpwm_generator_set_force_level(mcpwm_gen_left_a_handle, 0, true);
    mcpwm_generator_set_force_level(mcpwm_gen_left_b_handle, -1, true);
}

void bsp_mcpwm_right(uint32_t speed)
{
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr_right_handle, (uint32_t)(BSP_MCPWM_TIMER_PERIOD_TICKS * speed / 100U)));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr_left_handle, (uint32_t)(BSP_MCPWM_TIMER_PERIOD_TICKS * speed / 100U)));

    mcpwm_generator_set_force_level(mcpwm_gen_right_a_handle, 0, true);
    mcpwm_generator_set_force_level(mcpwm_gen_right_b_handle, -1, true);

    mcpwm_generator_set_force_level(mcpwm_gen_left_a_handle, -1, true);
    mcpwm_generator_set_force_level(mcpwm_gen_left_b_handle, 0, true);
}

void bsp_mcpwm_left(uint32_t speed)
{
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr_right_handle, (uint32_t)(BSP_MCPWM_TIMER_PERIOD_TICKS * speed / 100U)));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr_left_handle, (uint32_t)(BSP_MCPWM_TIMER_PERIOD_TICKS * speed / 100U)));

    mcpwm_generator_set_force_level(mcpwm_gen_right_a_handle, -1, true);
    mcpwm_generator_set_force_level(mcpwm_gen_right_b_handle, 0, true);

    mcpwm_generator_set_force_level(mcpwm_gen_left_a_handle, 0, true);
    mcpwm_generator_set_force_level(mcpwm_gen_left_b_handle, -1, true);
}