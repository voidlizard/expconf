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

typedef enum {
    AT_INT
  , AT_CSTRING
  , AT_OBJECT
  , AT_VOID
} argtype;

struct binding {
    void   *callee_cc;
    void   *callee;
    uint8_t arity;
    uint8_t targs[MAX_ARGS+1];
};


struct vlist {
    struct vlist *next;
    cell_t val;
};

#define CALLN(N, b, cc, args) ((ubound_fun_##N)(b)->callee)((cc) VL##N(args))

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

static inline cell_t wrap_string(char *s) {
    cell_t c = { .cs = s };
    return c;
}

static inline cell_t wrap_arg(argtype t, void *val) {
    cell_t v = { .vp = 0 };
    return v;
}

static inline void * unwrap_arg(argtype t, cell_t val) {
    return val.vp;
}

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
    return a + b;
}

#define RET_void(exp) do { exp; return (cell_t)(void*)0; } while(0)
#define RET_int(exp)  do { cell_t r = { .i = exp }; return r; } while(0)

#define RET_long      RET_int

/*#define WRAPPED_ARGS(...) WRAPPED_ARGS_(VA_LENGTH(__VA_ARGS__))*/
/*#define WRAPPED_ARGS_(n)  WRAPPED_ARGS__(n)*/
/*#define WRAPPED_ARGS__(n) _WRAPPED_ARGS_##n*/
/*#define _WRAPPED_ARGS_0*/
/*#define _WRAPPED_ARGS_1 , cell_t c1*/
/*#define _WRAPPED_ARGS_2 _WRAPPED_ARGS_1, cell_t c2*/
/*#define _WRAPPED_ARGS_3 _WRAPPED_ARGS_2, cell_t c3*/
/*#define _WRAPPED_ARGS_4 _WRAPPED_ARGS_3, cell_t c4*/
/*#define _WRAPPED_ARGS_5 _WRAPPED_ARGS_4, cell_t c5*/
/*#define _WRAPPED_ARGS_6 _WRAPPED_ARGS_5, cell_t c6*/
/*#define _WRAPPED_ARGS_7 _WRAPPED_ARGS_6, cell_t c7*/
/*#define _WRAPPED_ARGS_8 _WRAPPED_ARGS_7, cell_t c8*/
/*#define _WRAPPED_ARGS_9 _WRAPPED_ARGS_8, cell_t c9*/
/*#define _WRAPPED_ARGS_10 _WRAPPED_ARGS_9, cell_t c10*/

#define WA(x) x
#define WRAPPED_ARGS(...) IFNOTNULL(PUTSEP,__VA_ARGS__) ITERATE(WA, __VA_ARGS__)

#define DECL_WRAPPER(fun, ret, ...) \
static cell_t fun##__wrapper(void *cc WRAPPED_ARGS(__VA_ARGS__) ) {\
    struct binding *b = cc;\
    ret (*fn)(void*,##__VA_ARGS__) = b->callee;\
    RET_##ret(fn(b->callee_cc));\
}

#define P(x) X##x
#define JOPA(...) IFNOTNULL(PUTSEP,__VA_ARGS__) ITERATE(P, __VA_ARGS__)

VA_LENGTH(): JOPA()
VA_LENGTH(1): JOPA(1)
VA_LENGTH(1,2,3): JOPA(1,2,3)

DECL_WRAPPER(succ, int, int)

int main(int argc, char *argv[]) {

    struct binding bs[] = { } ;
    return 0;
}


