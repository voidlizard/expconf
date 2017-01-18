#ifndef __ulisp_macro_h
#define __ulisp_macro_h

#include <chaos/preprocessor.h>

#define CELLTYPE(...) struct ucell*

#define CELLVAL(x,y,cc) utuple_get(u, cc, CHAOS_PP_INC(y))


#define PRIMOPCALLEE(N, W, OP) \
    struct ucell* (*W)(struct ulisp*, struct ulisp_primop* CHAOS_PP_COMMA_IF(N)\
                         CHAOS_PP_EXPR(CHAOS_PP_DELINEATE_FROM_TO_PARAMETRIC(0, N, CHAOS_PP_COMMA, CELLTYPE, 0))\
                         ) = op->wrapper

#define MKPRIMOPCALLOF(n,tuple) {\
struct ulisp_primop *op = uprimop_val(utuple_get(u, tuple, 0));\
PRIMOPCALLEE(n, wrap, op);\
return wrap(u, op CHAOS_PP_COMMA_IF(n)\
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


#define ULISP_WRAPPER_NAME(name) __##name##_ulisp

#define ULISP_WRAP(type,val) ULISP_WRAP_(type,val)
#define ULISP_WRAP_(type,val) ULISP_WRAP##__##type(val)
#define ULISP_WRAP__int(value) integer(u, (size_t)value)

#define ULISP_UNWRAP(_0,i,type) ULISP_UNWRAP_(type, arg##i)
#define ULISP_UNWRAP_(t,n) ULISP_UNWRAP__##t(n)

#define ULISP_UNWRAP__int(n) ucell_int(n)

#define CELLCELL(_,i,_1) ucell_t* arg##i

#define COMMA_PREP(...) CHAOS_PP_COMMA_IF(CHAOS_PP_LIST_IS_NIL(__VA_ARGS__))

#define UNWRAPPED_ARGS(...) CHAOS_PP_TUPLE_AUTO_FOR_EACH_I(ULISP_UNWRAP, (__VA_ARGS__))

#define ULISP_WRAPPER(fun,ret,...) \
static ucell_t* ULISP_WRAPPER_NAME(fun)( struct ulisp *u\
                                       , struct ulisp_primop *op\
                                       COMMA_PREP(__VA_ARGS__)\
                                       CHAOS_PP_TUPLE_AUTO_FOR_EACH_I(CELLCELL, (__VA_ARGS__))\
                                       ) {\
    ret (*call)(void* COMMA_PREP(__VA_ARGS__) __VA_ARGS__) = (op)->callee;\
    return ULISP_WRAP(ret, callee((op->cc) COMMA_PREP(__VA_ARGS__) UNWRAPPED_ARGS(__VA_ARGS__) ));\
}\


#endif
