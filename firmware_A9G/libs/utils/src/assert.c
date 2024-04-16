
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char outBuffer[256];

void Assert(bool valid, const char *fmt)
{
    if (!valid)
        __assert(fmt);
}
