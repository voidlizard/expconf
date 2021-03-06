#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "ulisp.h"
#include "ulisp_primop_string.h"
#include "ulisp_bind.h"
#include "ulisp_parser.h"
#include "static_mem_pool.h"
#include "examples_common.h"

static void show_parse_error( void *cc
                            , ulisp_parser_err err
                            , eval_context ectx
                            , const char *misc) {

    fprintf(stderr, "%s error (parse): %s; %s\n", (const char*)ectx, ulisp_parse_err_str(err), misc);
}

static void on_eval_error( void *cc
                         , ulisp_eval_err err
                         , eval_context ectx
                         , const char *msg ) {

    const char *ctx = ectx;
    fprintf(stderr, "%s error: (eval, %s) : %s\n", ctx, ulisp_eval_err_str(err), msg);
}

static ucellp_t getenv_safe(object u, cstr name) {
    char *env = getenv((char*)name);
    if( !env ) return nil;
    return cstring_(u, env);
}

static int succ(int n) {
    return n+1;
}

static int sum(int a, int b) {
    return a+b;
}

static void print_strln(cstr str) {
    fprintf(stdout, "%s\n", str);
}

static void print_strln2(cstr str1, cstr str2) {
    fprintf(stdout, "%s%s\n", str1, str2);
}

// polymorphic function example
static void display(object u_, ucellp_t what) {
    fprintf(stdout, "%s", ustring_cstr(ucell_to_string(u_, what)));
}


static int sumlist(object u_, ucellp_t what) {
    fprintf(stderr, "I'm a sumlist function %p %s\n", what, ustring_cstr(ucell_to_string(u_, what)));
    return 0;
}

static void newline() {
    fprintf(stdout, "\n");
}

static void nomem(void *u_) {
    fprintf(stderr, "*** error (runtime): out of memory\n");
    assert(0);
}

ULISP_PRIMOP(succ,int,int)
ULISP_PRIMOP(sum,int,int,int)
ULISP_PRIMOP(print_strln, void, cstr)
ULISP_PRIMOP(print_strln2, void, cstr, cstr)
ULISP_PRIMOP(display, void, object, ucellp_t)
ULISP_PRIMOP(newline, void)
ULISP_PRIMOP(getenv_safe, ucellp_t, object, cstr)
ULISP_PRIMOP(getenv, cstr, cstr) // from stdlib.h !
ULISP_PRIMOP(sumlist, int, object, ucellp_t)

int main(int argc, char *argv[]) {

    if( argc < 2 ) {
        fprintf(stderr, "usage: ulisp_example file-name\n");
        exit(1);
    }

    int errors = 0;

    const char *fname = argv[1];

    char ulisp_mem[ulisp_size()];

    struct static_mem_pool pool;

    void *pp = static_mem_pool_init( &pool
                                   , 8192 - sizeof(struct static_mem_pool)
                                   , 0
                                   , nomem
                                   , 0
                                   , example_alloc
                                   , example_dealloc );


    FILE *file = 0;
    struct ulisp_parser *p = 0;

    jmp_buf env;
    if( setjmp(env) ) {
        errors++;
        goto _fin;
    }

    struct ulisp *u =  ulisp_create( ulisp_mem
                                   , sizeof(ulisp_mem)
                                   , &env
                                   , 0
                                   , on_eval_error
                                   , pp
                                   , static_mem_pool_alloc
                                   , static_mem_pool_dealloc
                                   );
    assert( u );

    do {

        struct ucell *binds =
            list(u, bind(u, "println",  closure(u, primop(u, &ULISP_PRIMOP_VAR(print_strln)), 0))
                          , bind(u, "println2", closure(u, primop(u, &ULISP_PRIMOP_VAR(print_strln2)), 1, cstring(u,"FREE1 ")))
                          , bind(u, "println3", closure(u, primop(u, &ULISP_PRIMOP_VAR(print_strln2)), 0))
                          , bind(u, "println4", closure(u, primop(u, &ULISP_PRIMOP_VAR(print_strln2)), 2, cstring(u,"A"), cstring(u,"D")))
    /*                      , bind(u, "getenv",   closure(u, primop(u, &ULISP_PRIMOP_VAR(getenv_safe)), 1, object(u,u)))*/
                          , bind(u, "getenv",   closure(u, primop(u, &ULISP_PRIMOP_VAR(getenv)), 0))
                          , bind(u, "display",  closure(u, primop(u, &ULISP_PRIMOP_VAR(display)), 1, object(u,u)))
                          , bind(u, "newline",  closure(u, primop(u, &ULISP_PRIMOP_VAR(newline)), 0))
                          , bind(u, "succ",     closure(u, primop(u, &ULISP_PRIMOP_VAR(succ)), 0))
                          , bind(u, "+",        closure(u, primop(u, &ULISP_PRIMOP_VAR(sum)), 0))
                          , bind(u, "++",       closure(u, primop_l(u, &ULISP_PRIMOP_VAR(sumlist)), 1, object(u,u)))
                          , bind(u, "__VERSION__", cstring(u, "ulisp-0.1-alpha"))
                          , bind(u, "__UNIT__", cstring(u, fname))
                          , nil);

        ulisp_bind(u, binds);

        char pmem[ulisp_parser_size()];
        p = ulisp_parser_create( pmem
                               , sizeof(pmem)
                               , file_read_char
                               , 0
                               , show_parse_error
                               , 0
                               , example_alloc
                               , example_dealloc
                               , u );


        file = fopen(fname, "rb");

        if( !file ) {
            fprintf(stderr, "*** fatal. %s: %s \n", fname, strerror(errno));
            exit(-1);
        }

        struct ucell *top = ulisp_parse_top(p, file);
        ulisp_parser_destroy(p);

        if( !top ) errors++;

        ulisp_eval_top(u, top);

    } while(0);

_fin:

    if( u ) ulisp_destroy(u);
    if( file) fclose(file);
    static_mem_pool_destroy(pp);

    return !errors ? 0 : -1;
}

