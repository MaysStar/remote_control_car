#include "bsp_log_hook.h"

/* Private variables and functions */
static volatile uint32_t log_stack_head = 0;
static volatile uint32_t log_stack_tail = 0;
static volatile char log_stack[BSP_STACK_SIZE][BSP_STACK_LOG_SIZE];

static int new_remote_log_fun(const char *frm, va_list list);
static volatile vprintf_like_t old_uart_log_fun = NULL;

static int new_remote_log_fun(const char *frm, va_list list)
{
    /* Copy data from log */
    va_list list_copy;
    va_copy(list_copy, list);

    /* Send data to default uart0 */
    assert(old_uart_log_fun != NULL);
    int res = old_uart_log_fun(frm, list);

    /* Format log and send in stack */
    vsnprintf(log_stack[log_stack_tail], BSP_STACK_LOG_SIZE, frm, list_copy);

    /* Avoid overflow */
    log_stack_tail++;
    if(log_stack_tail >= BSP_STACK_SIZE)
    {
        log_stack_tail = 0;
    }

    return res;
}

/* Change old log function to new. Send logs in uart0 and write into buffer */
void bsp_log_hook_init(void)
{
    /* Return old function and set new */
    old_uart_log_fun = esp_log_set_vprintf(new_remote_log_fun);
}

uint32_t bsp_get_new_logs_number(void)
{
    /* Avoid overflow and negative numbers */
    return (log_stack_tail - log_stack_head + BSP_STACK_SIZE) % BSP_STACK_SIZE;
}

void bsp_get_new_log(char* log, uint32_t log_size)
{
    if(log_stack_head >= BSP_STACK_SIZE)
    {
        log_stack_head = 0;
    }

    /* Get logs from begin */
    snprintf(log, (size_t)log_size, "%s", log_stack[log_stack_head]);
    log_stack_head++;
}