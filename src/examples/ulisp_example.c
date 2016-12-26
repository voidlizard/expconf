#include <stdio.h>
#include "ulisp.h"
#include "examples_common.h"

void print_nil( void *cc ) {
    fprintf(stdout, "NIL");
}

void print_int( void *cc, integer v ) {
    fprintf(stdout, "(int %ld)", v);
}

void print_list_start( void *cc ) {
    fprintf(stdout, "(");
}

void print_list_end( void *cc ) {
    fprintf(stdout, ")");
}

int main(int argc, char *argv[]) {

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
                              , .on_nil        = print_nil
                              };

    struct ucell *cell = list(u, mkinteger(u, 42)
                               , mkinteger(u, 43)
                               , mkinteger(u, 44)
                               , mkinteger(u, 45)
                               , nil);

    ucell_walk(u, cell, &cb);
    fprintf(stdout, "\n\n");

    ulisp_destroy(u);

    return 0;
}

