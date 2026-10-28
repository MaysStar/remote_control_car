#ifndef APP_COMMAND_PARSER_H
#define APP_COMMAND_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define APP_COMMAND_MAX_DECIMAL_DIGITS 3U

/* Parse an entire length-delimited unsigned decimal field.
 * On failure, leave the output unchanged. No terminator is required.
 */
static inline bool app_parse_decimal(const char *data, size_t length, unsigned maximum,
                                     uint8_t *result)
{
    if (data == NULL || result == NULL || length == 0U || length > APP_COMMAND_MAX_DECIMAL_DIGITS ||
        maximum > UINT8_MAX)
    {
        return false;
    }

    unsigned value = 0U;

    for (size_t index = 0U; index < length; ++index)
    {
        if (data[index] < '0' || data[index] > '9')
        {
            return false;
        }

        value = value * 10U + (unsigned)(data[index] - '0');
    }

    if (value > maximum)
    {
        return false;
    }

    *result = (uint8_t)value;
    return true;
}

#endif /* APP_COMMAND_PARSER_H */
