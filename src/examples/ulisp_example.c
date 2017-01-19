#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "ulisp.h"
#include "ulisp_bind.h"
#include "examples_common.h"

void print_nil( void *cc ) {
    fprintf(stdout, "#nil");
}

void print_int( void *cc, integer v ) {
    fprintf(stdout, "%ld ", v);
}

void print_str(void *cc, size_t len, const char *s) {
    fprintf(stdout, "\"%s\" ", s);
}

void print_atom(void *cc, size_t len, const char *s) {
    fprintf(stdout, "%s ", s);
}

void print_list_start( void *cc ) {
    fprintf(stdout, "(cons ");
}

void print_list_end( void *cc ) {
    fprintf(stdout, ") ");
}

static void show_parse_error( void *cc
                            , ulisp_parser_err err
                            , size_t lno
                            , char *misc) {

    fprintf(stderr, "*** error (parse) %ld: %s\n", lno, ulisp_parse_err_str(err));
}


static struct ucell_walk_cb walk_cb = { .cc = 0
                                      , .on_list_start = print_list_start
                                      , .on_list_end   = print_list_end
                                      , .on_integer    = print_int
                                      , .on_atom       = print_atom
                                      , .on_string     = print_str
                                      , .on_nil        = print_nil
                                      };

static ucell_t *__display( struct ulisp *u, ucell_t *op, ucell_t *rs ) {
    fprintf(stdout, "%s\n", ustring_cstr(rs));
    return nil;
}


static struct ulisp_primop  __primop_display = { .arity = 1
                                               , .callee = 0
                                               , .wrapper = __display
                                               , .tp = UNIT
                                               , .argtp = { STRING }
                                               };


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

static void newline() {
    fprintf(stdout, "\n");
}

ULISP_WRAPPER_DECL(succ,int,int)
ULISP_WRAPPER_DECL(sum,int,int,int)
ULISP_WRAPPER_DECL(print_strln, void, cstr)
ULISP_WRAPPER_DECL(print_strln2, void, cstr, cstr)
ULISP_WRAPPER_DECL(display, void, object, ucellp_t)
ULISP_WRAPPER_DECL(newline, void)
ULISP_WRAPPER_DECL(getenv_safe, ucellp_t, object, cstr)

int main(int argc, char *argv[]) {

    if( argc < 2 ) {
        fprintf(stderr, "usage: ulisp_example file-name\n");
        exit(1);
    }

    const char *fname = argv[1];

    char ulisp_mem[ulisp_size()];

    struct ulisp *u = ulisp_create( ulisp_mem
                                  , sizeof(ulisp_mem)
                                  , 0
                                  , example_alloc
                                  , example_dealloc
                                  );


    assert( u );

    struct ucell *binds = list(u, bind(u, "println",  closure(u, primop(u, &ULISP_PRIMOP_VAR(print_strln)), 0))
                                , bind(u, "println2", closure(u, primop(u, &ULISP_PRIMOP_VAR(print_strln2)), 1, cstring(u,"FREE1 ")))
                                , bind(u, "println3", closure(u, primop(u, &ULISP_PRIMOP_VAR(print_strln2)), 0))
                                , bind(u, "println4", closure(u, primop(u, &ULISP_PRIMOP_VAR(print_strln2)), 2, cstring(u,"A"), cstring(u,"B")))

                                , bind(u, "getenv",   closure(u, primop(u, &ULISP_PRIMOP_VAR(getenv_safe)), 1, object(u,u)))
                                , bind(u, "display",  closure(u, primop(u, &ULISP_PRIMOP_VAR(display)), 1, object(u,u)))
                                , bind(u, "newline",  closure(u, primop(u, &ULISP_PRIMOP_VAR(newline)), 0))
                                , bind(u, "succ",     closure(u, primop(u, &ULISP_PRIMOP_VAR(succ)), 0))
                                , bind(u, "+",        closure(u, primop(u, &ULISP_PRIMOP_VAR(sum)), 0))
                                , bind(u, "__VERSION__", cstring(u, "ulisp-0.1-alpha"))
                                , nil);

    ulisp_bind(u, binds);

    char pmem[ulisp_parser_size()];

    struct ulisp_parser *p = ulisp_parser_create( pmem
                                                , sizeof(pmem)
                                                , file_read_char
                                                , 0
                                                , show_parse_error
                                                , 0
                                                , example_alloc
                                                , example_dealloc
                                                , u );


    FILE *file = fopen(fname, "rb");

    if( !file ) {
        fprintf(stderr, "*** fatal. %s: %s \n", fname, strerror(errno));
        exit(-1);
    }

    struct ucell *top = ulisp_parse_top(p, file);

    ulisp_eval_top(u, top);

__exit:

    ulisp_parser_destroy(p);
    ulisp_destroy(u);
    fclose(file);

    return 0;
}

