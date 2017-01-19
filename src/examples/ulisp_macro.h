#ifndef __ulisp_macro_h
#define __ulisp_macro_h

#include <chaos/preprocessor.h>

#define CELLTYPE(...) struct ucell*

#define CELLVAL(x,y,cc) utuple_get(u, cc, CHAOS_PP_INC(y))

#define PRIMOPCALLEE(N, W) \
    struct ucell* (*W)( struct ulisp*\
                      , struct ulisp_primop*\
                        CHAOS_PP_COMMA_IF(N)\
                        CHAOS_PP_EXPR(CHAOS_PP_DELINEATE_FROM_TO_PARAMETRIC(0, N, CHAOS_PP_COMMA, CELLTYPE, 0))\
                      ) = op->wrapper

#define MKPRIMOPCALLOF(n,tuple) {\
PRIMOPCALLEE(n, wrap);\
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
    struct ulisp_primop *op = ucell_primop(utuple_get(u, (tuple), 0));\
    switch( op->arity ) {\
        CHAOS_PP_EXPR(CHAOS_PP_REPEAT_FROM_TO(0, N, MKPRIMOPCALL, tuple))\
        default:\
            fprintf(stderr, "*** error (runtime): unsupported tuple size: %d\n", op->arity);\
            assert(0);\
    }\
} while(0)


#define ULISP_WRAPPER_NAME(name) __##name##_ulisp
#define ULISP_PRIMOP_VAR(name)   __##name##_primop

#define ULISP_WRAP(type,val) ULISP_WRAP_(type,val)
#define ULISP_WRAP_(type,val) ULISP_WRAP##__##type(val)
#define ULISP_WRAP__int(value) integer(u, (size_t)(value))
#define ULISP_WRAP__cstr(value) cstring(u, value)
#define ULISP_WRAP__void(value) (value), nil

#define ULISP_UNWRAP(_0,i,type) ULISP_UNWRAP_(type, arg##i)
#define ULISP_UNWRAP_(t,n) ULISP_UNWRAP__##t(n)

#define ULISP_UNWRAP__int(n) ucell_intval((n))
#define ULISP_UNWRAP__cstr(n) ustring_cstr((n))

#define ULISP_WRAP_TYPE(tp) ULISP_WRAP_TYPE_(tp)
#define ULISP_WRAP_TYPE_(tp) ULISP_WRAP_TYPE__##tp

#define ULISP_WRAP_TYPE__void UNIT
#define ULISP_WRAP_TYPE__int INTEGER
#define ULISP_WRAP_TYPE__primop PRIMOP
#define ULISP_WRAP_TYPE__cstr STRING

#define ULISP_UNWRAP_SEP(_0,i,type) CHAOS_PP_COMMA_IF(i) ULISP_UNWRAP(_0,i,type)

#define WRAPTYPE(_,i,type) CHAOS_PP_COMMA_IF(i) ULISP_WRAP_TYPE(type)
#define CELLCELL(_,i,_1) ,ucell_t* arg##i

#define COMMA_PREP(...) CHAOS_PP_COMMA_IF(CHAOS_PP_GREATER(CHAOS_PP_TUPLE_SIZE((__VA_ARGS__)),1))

#define UNWRAPPED_ARGS(...) CHAOS_PP_TUPLE_AUTO_FOR_EACH_I(ULISP_UNWRAP_SEP, (__VA_ARGS__))




#define ULISP_WRAPPER_DECL(fun,ret,...) \
static ucell_t* ULISP_WRAPPER_NAME(fun)( struct ulisp *u\
                                       , struct ulisp_primop *op\
                                       CHAOS_PP_TUPLE_AUTO_FOR_EACH_I(CELLCELL, (__VA_ARGS__))\
                                       ) {\
    ret (*call)(__VA_ARGS__) = (op)->callee;\
    return ULISP_WRAP(ret, call(UNWRAPPED_ARGS(__VA_ARGS__)));\
}\
static struct ulisp_primop ULISP_PRIMOP_VAR(fun) = {\
    .arity = CHAOS_PP_TUPLE_SIZE((__VA_ARGS__))\
  , .callee = fun\
  , .wrapper = ULISP_WRAPPER_NAME(fun)\
  , .tp = ULISP_WRAP_TYPE(ret)\
  , .argtp = { CHAOS_PP_TUPLE_AUTO_FOR_EACH_I(WRAPTYPE, (__VA_ARGS__)) }\
};

#endif
