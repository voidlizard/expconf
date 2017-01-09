#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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


void print_cs(void *cc, char *s) {
    fprintf(stdout, "%s\n", s);
}

void* print_cs_wrap(void *cc, cell_t arg1) {
    print_cs(cc, arg1.cs);
    return 0;
}

void* print_cs2_wrap(void *cc, cell_t arg1, cell_t arg2) {
    fprintf(stdout, "%s%s\n", arg1.cs, arg2.cs);
    return 0;
}

void* print_cs3_wrap(void *cc, cell_t arg1, cell_t arg2, cell_t arg3) {
    fprintf(stdout, "%s%s%s\n", arg1.cs, arg2.cs, arg3.cs);
    return 0;
}

void* print_cs4_wrap(void *cc, cell_t arg1, cell_t arg2, cell_t arg3, cell_t arg4) {
    fprintf(stdout, "%s%s%s%s\n", arg1.cs, arg2.cs, arg3.cs, arg4.cs);
    return 0;
}

int main(int argc, char *argv[]) {

    struct binding bs[] = {   { .callee = print_cs_wrap
                              , .arity = 1
                              , .targs[0] = AT_CSTRING
                              , .targs[1] = AT_VOID
                              }
                            , { .callee = print_cs2_wrap
                              , .arity = 2
                              , .targs[0] = AT_CSTRING
                              , .targs[1] = AT_CSTRING
                              , .targs[2] = AT_VOID
                              }
                            , { .callee = print_cs3_wrap
                              , .arity = 3
                              , .targs[0] = AT_CSTRING
                              , .targs[1] = AT_CSTRING
                              , .targs[2] = AT_CSTRING
                              , .targs[3] = AT_VOID
                              }
                            , { .callee = print_cs4_wrap
                              , .arity = 4
                              , .targs[0] = AT_CSTRING
                              , .targs[1] = AT_CSTRING
                              , .targs[2] = AT_CSTRING
                              , .targs[3] = AT_CSTRING
                              , .targs[3] = AT_VOID
                              }
                          };


    struct vlist v3 = { .next = 0
                      , .val = wrap_string("TRESKI")
                      };

    struct vlist v2 = { .next = &v3
                      , .val = wrap_string("PECHEN")
                      };

    struct vlist v1 = { .next = &v2
                      , .val = wrap_string("KITA")
                      };

    struct vlist v0 = { .next = &v1
                      , .val = wrap_string("JOPA")
                      };

    call_with_arglist(0, &bs[0], &v0);
    call_with_arglist(0, &bs[1], &v0);
    call_with_arglist(0, &bs[2], &v0);
    call_with_arglist(0, &bs[3], &v0);

    return 0;
}






