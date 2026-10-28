#include "app_command_parser.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    uint8_t value = 77;
    assert(app_parse_decimal("0", 1, 100, &value) && value == 0);
    assert(app_parse_decimal("100", 3, 100, &value) && value == 100);
    assert(app_parse_decimal("4", 1, 4, &value) && value == 4);
    const char *invalid[] = {"", "-1", "+1", " 1", "1 ", "1x", "101", "999", "1000"};
    for (unsigned i = 0; i < sizeof(invalid)/sizeof(*invalid); ++i) {
        size_t len = 0; while (invalid[i][len]) ++len;
        assert(!app_parse_decimal(invalid[i], len, 100, &value));
    }
    assert(!app_parse_decimal("5", 1, 4, &value));
    assert(!app_parse_decimal("1\0x", 3, 100, &value));
    assert(!app_parse_decimal(NULL, 1, 100, &value));
    assert(!app_parse_decimal("1", 1, 100, NULL));
    char bounded[] = {'9','0','x'};
    assert(app_parse_decimal(bounded, 2, 100, &value) && value == 90);
    puts("command parser tests passed");
}
