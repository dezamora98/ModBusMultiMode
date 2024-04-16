
#ifndef _ASSERT_H_
#define _ASSERT_H_

#include "sdk_init.h"

#define __assert CSDK_FUNC(__assert)

void Assert(bool valid, const char *fmt);

#define assert(valid) \
do {\
\
    extern char outBuffer[256];\
    sprintf(outBuffer, "FILE:(%s) | LINE:(%d)", __FILE__, __LINE__);\
    if (!(valid))\
        __assert(outBuffer);\
} while(0)

#endif
