#include <stdio.h>
#include "ulisp.h"
#include "examples_common.h"

void print_nil( void *cc ) {
    fprintf(stdout, "nil");
}

void print_int( void *cc, integer v ) {
    fprintf(stdout, "(int %ld)", v);
}

void print_str(void *cc, size_t len, const char *s) {
    fprintf(stdout, "(string %ld \"%s\")", len, s);
}

void print_atom(void *cc, size_t len, const char *s) {
    fprintf(stdout, "(atom %s)", s);
}

void print_list_start( void *cc ) {
    fprintf(stdout, "(");
}

void print_list_end( void *cc ) {
    fprintf(stdout, ")");
}

/*void apply_primop( struct ulisp *l*/
/*                 , struct ucell *cell) {*/


/*    switch(arity) {*/
/*        case 0:*/
/*            assert(0);*/
/*            break;*/

/*        case 1:*/
/*            primop(car(cell));*/
/*            assert(0);*/
/*            break;*/

/*        case 2:*/
/*            primop( car(cell)*/
/*                  , car(cdr(cell))*/
/*                  );*/
/*            assert(0);*/
/*            break;*/

/*        case 3:*/
/*            primop( car(cell)*/
/*                  , car(cdr(cell))*/
/*                  , car(cdr(cdr(cell)))*/
/*                  );*/
/*            assert(0);*/
/*            break;*/
/*    }*/

/*}*/

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
                              , .on_atom       = print_atom
                              , .on_string     = print_str
                              , .on_nil        = print_nil
                              };

/*    struct ucell *cell = list(u, primop(u, "+", op_add, 2, INTEGER, INTEGER)*/
/*                               , nil*/
/*                             )*/

    struct ucell *cell = list(u, mkatom(u, "some-atom")
                               , mkinteger(u, 42)
                               , mkinteger(u, 43)
                               , mkinteger(u, 44)
                               , mkinteger(u, 45)
                               , mkinteger(u, 46)
                               , mkcstring(u, "TESTSTRING1")
                               , mkcstring(u, "TESTSTRING1")
                               , mkcstring(u, "TESTSTRING1")
                               , mkcstring(u, "TESTSTRING1")
                               , mkcstring(u, "TESTSTRING2")
                               , mkcstring(u, "TESTSTRING3")
                               , nil);

    ucell_walk(u, cell, &cb);
    fprintf(stdout, "\n\n");

    ulisp_destroy(u);

    return 0;
}

