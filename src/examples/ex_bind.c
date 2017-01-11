#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cpp_sodomy.h"

#undef ISEP
#define ISEP ,

typedef union {
    void *vp;
    char *cs;
    int    i;
} cell_t;

#define MAX_ARGS 10

#define ARG_LIST_1 cell_t
#define ARG_LIST_2 ARG_LIST_1, cell_t
#define ARG_LIST_3 ARG_LIST_2, cell_t
#define ARG_LIST_4 ARG_LIST_3, cell_t
#define ARG_LIST_5 ARG_LIST_4, cell_t
#define ARG_LIST_6 ARG_LIST_5, cell_t
#define ARG_LIST_7 ARG_LIST_6, cell_t
#define ARG_LIST_8 ARG_LIST_7, cell_t
#define ARG_LIST_9 ARG_LIST_8, cell_t
#define ARG_LIST_10 ARG_LIST_9, cell_t

#define NIL ((cell_t)(void*)0)
#define CAR(x) ((x)?(x)->val:NIL)
#define CDR(x) ((x)?((x)->next):0)

#define CDR1(x)  CDR(x)
#define CDR2(x)  CDR1(CDR1(x))
#define CDR3(x)  CDR1(CDR2(x))
#define CDR4(x)  CDR1(CDR3(x))
#define CDR5(x)  CDR1(CDR4(x))
#define CDR6(x)  CDR1(CDR5(x))
#define CDR7(x)  CDR1(CDR6(x))
#define CDR8(x)  CDR1(CDR7(x))
#define CDR9(x)  CDR1(CDR8(x))
#define CDR10(x) CDR1(CDR9(x))

#define VL0(x)
#define VL1(x) ,CAR(x)
#define VL2(x)  VL1(x), CAR(CDR1(x))
#define VL3(x)  VL2(x), CAR(CDR2(x))
#define VL4(x)  VL3(x), CAR(CDR3(x))
#define VL5(x)  VL4(x), CAR(CDR4(x))
#define VL6(x)  VL5(x), CAR(CDR5(x))
#define VL7(x)  VL6(x), CAR(CDR7(x))
#define VL8(x)  VL7(x), CAR(CDR(x))
#define VL9(x)  VL8(x), CAR(CDR9(x))
#define VL10(x) VL9(x), CAR(CDR10(x))

typedef cell_t (*ubound_fun_0)(void*);
typedef cell_t (*ubound_fun_1)(void*, ARG_LIST_1);
typedef cell_t (*ubound_fun_2)(void*, ARG_LIST_2);
typedef cell_t (*ubound_fun_3)(void*, ARG_LIST_3);
typedef cell_t (*ubound_fun_4)(void*, ARG_LIST_4);
typedef cell_t (*ubound_fun_5)(void*, ARG_LIST_5);
typedef cell_t (*ubound_fun_6)(void*, ARG_LIST_6);
typedef cell_t (*ubound_fun_7)(void*, ARG_LIST_7);
typedef cell_t (*ubound_fun_8)(void*, ARG_LIST_8);
typedef cell_t (*ubound_fun_9)(void*, ARG_LIST_9);
typedef cell_t (*ubound_fun_10)(void*, ARG_LIST_10);

struct smth {
};

typedef char* cstring;
typedef char* cstr;

typedef enum {
    AT_INT
  , AT_CSTRING
  , AT_OBJECT
  , AT_VOID
} argtype;

typedef struct {
    void *cc;
    void *callee;
} callee_t;

struct binding {
    void    *wrapper;
    callee_t callee;
    uint8_t arity;
    uint8_t targs[MAX_ARGS+1];
};


struct vlist {
    struct vlist *next;
    cell_t val;
};

#define CALLN(N, b, cc, args) ((ubound_fun_##N)(b)->wrapper)((b) VL##N(args))

cell_t call_with_arglist( void *cc
                        , struct binding *b
                        , struct vlist *al ) {

    cell_t nil = { .vp = 0 };

    switch( b->arity ) {
        case 0: return CALLN(0, b, cc, al);
        case 1: return CALLN(1, b, cc, al);
        case 2: return CALLN(2, b, cc, al);
        case 3: return CALLN(3, b, cc, al);
        case 4: return CALLN(4, b, cc, al);
        case 5: return CALLN(5, b, cc, al);
        case 6: return CALLN(6, b, cc, al);
        case 7: return CALLN(7, b, cc, al);
        case 8: return CALLN(8, b, cc, al);
        case 9: return CALLN(9, b, cc, al);
        case 10: return CALLN(10, b, cc, al);
        default:
             fprintf(stderr, "bad arity\n");
             exit(-1);
    }

    return nil;
}


#define RET_void(exp)    do { exp; return (cell_t)(void*)0; } while(0)
#define RET_int(exp)     do { cell_t r = { .i = exp }; return r; } while(0)
#define RET_object(exp)  do { cell_t r = { .vp = exp }; return r; } while(0)
#define RET_cstring(exp) do { cell_t r = { .cs = exp }; return r; } while(0)

#define RET_long(exp) RET_int(exp)

#define RET_ptr(exp) RET_object(exp)
#define RET_voidptr(exp) RET_object(exp)

#define RET_string(exp) RET_cstring(exp)
#define RET_str(exp) RET_cstring(exp)
#define RET_cstr(exp) RET_cstring(exp)
#define RET_pchar(exp) RET_cstring(exp)

#define WA(n,x) cell_t c##n
#define WRAPPED_ARGS(...) IFNOTNULL(PUTSEP,__VA_ARGS__) ITERATE(WA, __VA_ARGS__)

#define UA(n,t) UA_(c##n, t)
#define UA_(v, t) UA__(v,t)
#define UA__(v, t) _UA_##t(v)

#define _UA_cstring(v) v.cs
#define _UA_cstr(v) v.cs
#define _UA_int(v) v.i
#define _UA_object(v) v.vp
#define _UA_pchar(v) v.cs
#define _UA_ptr(v) v.vp
#define _UA_pvoid(v) v.vp
#define _UA_string(v) v.cs
#define _UA_str(v) v.cs
#define _UA_voidptr(v) v.vp

#define UNWRAPPED_ARGS(...) IFNOTNULL(PUTSEP,__VA_ARGS__) ITERATE(UA, __VA_ARGS__)

#define WRAPPER_NAME(fun) __wrap_##fun

#define BIND_VAR_NAME(fun) __bind_var_##fun

#define TYPE_TAG(n,t) TTAG_(t)
#define TTAG_(t) TTAG__##t

#define TTAG__void AT_VOID
#define TTAG__int  AT_INT
#define TTAG__string AT_CSTRING
#define TTAG__cstring AT_CSTRING
#define TTAG__pchar AT_CSTRING
#define TTAG__object AT_OBJECT
#define TTAG__pvoid  AT_OBJECT
#define TTAG__ptr AT_OBJECT


#define BIND(fun,ret,...) BIND_IMPL(fun,fun,ret,##__VA_ARGS__)
#define BINDN(n,f,r,...) BIND_IMPL(n,f,r,##__VA_ARGS__)

#define BIND_IMPL(name,fun, ret, ...) \
static cell_t WRAPPER_NAME(fun)(void *cc WRAPPED_ARGS(__VA_ARGS__) ) {\
    struct binding *b = cc;\
    ret (*fn)(void*,##__VA_ARGS__) = b->callee.callee;\
    RET_##ret(fn(b->callee.cc UNWRAPPED_ARGS(__VA_ARGS__) ));\
} \
\
static struct binding BIND_VAR_NAME(fun) =\
{ .wrapper = WRAPPER_NAME(fun)\
, .callee  = { .cc = 0, .callee = fun }\
, .arity   = VA_LENGTH(__VA_ARGS__) \
, .targs   = { ITERATE(TYPE_TAG, APPEND(ret,__VA_ARGS__)) }\
};\


void print0(void *cc) {
    fprintf(stdout, "\n");
}

void print_cs(void *cc, char *s) {
    fprintf(stdout, "%s\n", s);
}

int succ(void *cc, int a) {
    return a + 1;
}

int sum(void *cc, int a, int b) {
    fprintf(stderr, "debug: sum %d %d is %d\n", a, b, a+b);
    return a + b;
}

BIND(succ, int, int)
BIND(sum, int, int, int)
BIND(print0, void)
BIND(print_cs, void, cstring)

int main(int argc, char *argv[]) {

    struct binding *bs[] = { &BIND_VAR_NAME(print_cs)
                           , &BIND_VAR_NAME(print0)
                           , &BIND_VAR_NAME(sum)
                           };

    struct vlist vl0 = { .next = 0
                       , .val.cs = "JOPA"
                       };

    struct vlist i2  = { .next = 0
                       , .val.i = 3
                       };

    struct vlist i1  = { .next = &i2
                       , .val.i = 2
                       };

    call_with_arglist( 0, bs[0], &vl0 );
    call_with_arglist( 0, bs[1], &i1 );

    cell_t rs = call_with_arglist( 0, bs[2], &i1 );
    fprintf(stdout, "got cell %d\n", rs.i);

    return 0;
}

