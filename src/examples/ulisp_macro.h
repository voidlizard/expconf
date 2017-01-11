#ifndef __ulisp_macro_h
#define __ulisp_macro_h

#include <chaos/preprocessor.h>

#define CELLTYPE(...) struct ucell*

#define CELLVAL(x,y,cc) utuple_get(u, cc, CHAOS_PP_INC(y))

#define PRIMOPCALLEE(N, W, OP) \
    struct ucell* (*W)(struct ulisp *u CHAOS_PP_COMMA_IF(N)\
                         CHAOS_PP_EXPR(CHAOS_PP_DELINEATE_FROM_TO_PARAMETRIC(0, N, CHAOS_PP_COMMA, CELLTYPE, 0))\
                         ) = op->wrapper

#define MKPRIMOPCALLOF(n,tuple) {\
struct ulisp_primop *op = uprimop_val(utuple_get(u, tuple, 0));\
PRIMOPCALLEE(n, wrap, op);\
return wrap(u CHAOS_PP_COMMA_IF(n)\
              CHAOS_PP_EXPR(CHAOS_PP_DELINEATE_FROM_TO_PARAMETRIC(0, n, CHAOS_PP_COMMA, CELLVAL, tuple))\
           );\
}

#define MKPRIMOPCALL(x,n,cc) \
case n:\
MKPRIMOPCALLOF(n,cc)\
break;\

// FIXME: error handling
#define GENERATE_PRIMOP_CALL(N, tuple) \
do {\
    struct ulisp_primop *op = uprimop_val(utuple_get(u, (tuple), 0));\
    switch( op->arity ) {\
        CHAOS_PP_EXPR(CHAOS_PP_REPEAT_FROM_TO(0, N, MKPRIMOPCALL, tuple))\
        default:\
            fprintf(stderr, "*** error (runtime): unsupported tuple size: %d\n", op->arity);\
            assert(0);\
    }\
} while(0)

#endif
