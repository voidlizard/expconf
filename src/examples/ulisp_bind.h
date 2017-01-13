#ifndef __ulisp_bind_h
#define __ulisp_bind_h

#include "ulisp_types.h"

struct ulisp_primop {
    uint8_t arity;
    void *callee_cc;
    void *callee;
    void *wrapper;
    ucell_type tp;
    ucell_type argtp[ULISP_TUPLE_MAX];
};

#endif
