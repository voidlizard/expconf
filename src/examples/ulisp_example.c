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

    struct ucell_walk_cb cb = { .cc = 0
                              , .on_list_start = print_list_start
                              , .on_list_end   = print_list_end
                              , .on_integer    = print_int
                              , .on_atom       = print_atom
                              , .on_string     = print_str
                              , .on_nil        = print_nil
                              };

    struct ucell *cell = list(u, integer(u, 1)
                               , integer(u, 2)
                               , integer(u, 3)
                               , cstring(u, "JOPA")
                               , list(u, atom(u, "A"), atom(u, "B"), nil)
                               , atom(u, "end")
                               , nil );

    ucell_walk(u, cell, &cb);
    fprintf(stdout, "\n");

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

    ucell_walk(u, top, &cb);
    fprintf(stdout, "\n");

    ulisp_eval_top(u, top);

__exit:

    ulisp_parser_destroy(p);
    ulisp_destroy(u);
    fclose(file);

    return 0;
}

