#ifndef __ulisp_platform_h
#define __ulisp_platform_h

#include "ulisp_platform_default.h"

#include "allocators.h"

#define safeappv(e, ...) (e)(__VA_ARGS__)

#endif
