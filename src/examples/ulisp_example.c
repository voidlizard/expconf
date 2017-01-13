#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "ulisp.h"
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

    struct ucell *binds = list(u, bind(u, "i42", integer(u, 42))
                                , bind(u, "i43", integer(u, 43))
                                , nil
                              );

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

