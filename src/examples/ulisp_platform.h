#ifndef __ulisp_platform_h
#define __ulisp_platform_h

#include "ulisp_platform_default.h"

#include "allocators.h"
#include "stringlike.h"

#define safeappv(e, ...) (e)(__VA_ARGS__)

#define ULISP_LIST_LIT_MAX 1024
#define ULISP_TUPLE_MAX 64
#define ULISP_ATOM_MAX 255

#endif
