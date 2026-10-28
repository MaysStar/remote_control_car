#ifndef APP_COMMAND_PARSER_H
#define APP_COMMAND_PARSER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Length-delimited, allocation-free protocol boundary. Never accepts prefixes. */
static inline bool app_parse_decimal(const char *data, size_t len,
                                     unsigned maximum, uint8_t *result)
{
    if (!data || !result || len == 0 || len > 3) return false;
    unsigned value = 0;
    for (size_t i = 0; i < len; ++i) {
        if (data[i] < '0' || data[i] > '9') return false;
        value = value * 10 + (unsigned)(data[i] - '0');
    }
    if (value > maximum) return false;
    *result = (uint8_t)value;
    return true;
}
#endif
