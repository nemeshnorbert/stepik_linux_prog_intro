#include <stddef.h>
#include <string.h>
#include "solution.h"

int stringStat(const char *string, size_t multiplier, int *count) {
    *count += 1;
    return strlen(string) * multiplier;
}
