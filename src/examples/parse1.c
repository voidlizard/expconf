#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exp_tokenize.h"
#include "allocators.h"
#include "strchunk.h"
#include "examples_common.h"

typedef enum {
      INTEGER
    , ATOM
    , STRING
    , LIST
    , NIL
} exp_cell_type;

struct exp_cell {
    exp_cell_type tag;

    union {
        struct exp_cell *expr;
        integer          integer;
    } car;

    struct exp_cell *cdr;

    char celldata[0];
};

#define exp_nil 0

struct exp_parser {

    struct {
        struct exp_token   mem_curr;
        struct exp_token   mem_back;
        struct exp_token  *curr;
        struct exp_token  *back;
    } token;

    struct exp_tokenizer *tokenizer;
    void  *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
};


struct exp_cell *exp_cell_new( exp_cell_type t
                             , void *allocator
                             , alloc_function_t alloc) {


    size_t cell_size = sizeof(struct exp_cell);
    struct exp_cell *cell = alloc(allocator, cell_size);

    if( !cell ) {
        // FIXME: out of memory
        return exp_nil;
    }

    memset(cell, 0, cell_size);
    cell->tag = t;

    return cell;
}

#define EXP_ALLOC_CELL(cell, t, a, alloc) do {\
        (cell) = exp_cell_new((t), (a), (alloc));\
        if( !cell ) {\
            return 0;\
        }\
    }while(0)\


struct exp_cell* exp_integer( integer v
                            , void *allocator
                            , alloc_function_t alloc) {

    struct exp_cell *cell = 0;

    EXP_ALLOC_CELL(cell, INTEGER, allocator, alloc);

    cell->car.integer = v;

    return cell;
}

struct exp_cell* exp_atom( void *atom
                         , void *allocator
                         , alloc_function_t alloc ) {
    return exp_nil;
}

struct exp_cell* exp_list( struct exp_cell *car
                         , struct exp_cell *cdr
                         , void *allocator
                         , alloc_function_t alloc ) {


    struct exp_cell *cell = 0;

    EXP_ALLOC_CELL(cell, INTEGER, allocator, alloc);

    cell->tag = LIST;
    cell->car.expr = car;
    cell->cdr = cdr;

    return cell;
}

struct exp_parser *exp_parser_create( struct exp_parser *mem
                                    , void *reader
                                    , read_char_fn readfn
                                    , void *allocator
                                    , alloc_function_t alloc
                                    , dealloc_function_t dealloc ) {


    memset(mem, 0, sizeof(struct exp_parser));
    mem->tokenizer = exp_tokenizer_create( allocator
                                         , alloc
                                         , dealloc
                                         , reader
                                         , readfn
                                         );

    if( !mem->tokenizer ) {
        return 0;
    }

    mem->allocator = allocator;
    mem->alloc     = alloc;
    mem->dealloc   = dealloc;
    return mem;
}

void exp_parser_destroy( struct exp_parser **p ) {

    struct exp_parser *pp = *p;

    if( !pp ) {
        return;
    }

    exp_tokenizer_destroy( &pp->tokenizer );

    *p = 0;
}

struct exp_token *exp_parser_get_token(struct exp_parser* p) {

    if( p->token.back ) {
        struct exp_token *tok = p->token.back;
        p->token.back = 0;
        return tok;
    }

    p->token.curr = exp_tokenizer_next(p->tokenizer, &p->token.mem_curr);
    return p->token.curr;
}


struct exp_token *exp_parser_unget_token(struct exp_parser* p) {
    p->token.back = p->token.curr;
}

struct exp_cell *exp_parse_expr( struct exp_parser *p, struct exp_cell *top );

struct exp_cell *exp_parse_list(struct exp_parser *p) {

    struct exp_token *tok = exp_parser_get_token(p);

    if( !tok ) {
        // FIXME: parse error
        return exp_nil;
    }

    if( tok->tag == TOK_CPAREN ) {
        return exp_nil;
    }

    exp_parser_unget_token(p);

    struct exp_cell *car = exp_parse_expr(p, false);
    struct exp_cell *cdr = exp_parse_list(p);

    return exp_list(car, cdr, p->allocator, p->alloc);
}

struct exp_cell *exp_parse_expr( struct exp_parser *p, struct exp_cell *top ) {

    if( top ) {
        exp_list(top, exp_parse_list(p), p->allocator, p->alloc);
    }

    struct exp_token *tok = exp_parser_get_token(p);

    if( !tok ) {
        return exp_nil;
    }

    switch( tok->tag ) {
        case TOK_INTEGER:
            return exp_integer(tok->v.intval, p->allocator, p->alloc);

        case TOK_ATOM:
            // КУДА ДЕВАТЬ АТОМЫ?
            return exp_nil;

        case TOK_STRING:
            // КУДА ДЕВАТЬ СТРОКИ?
            return exp_nil;

        case TOK_OPAREN:
            return exp_parse_list(p);

        case TOK_CPAREN:
            return exp_nil;

        default:
            assert(0);
            // TODO: parse error?
            return exp_nil;
    }

    return exp_nil;
}

struct exp_cell *exp_parse(struct exp_parser *parser, struct exp_cell *top) {
    return exp_parse_expr(parser, top);
}


void dump_cell(void *cc, struct exp_cell *cell) {

    if( !cell ) {
        fprintf(stdout, "(nil)");
        return;
    }

    switch( cell->tag ) {
        case INTEGER:
            fprintf(stdout, "I#%ld ", cell->car.integer);
            break;
        case LIST:
            fprintf(stdout, "(");
            dump_cell(cc, cell->car.expr);
            dump_cell(cc, cell->cdr);
            fprintf(stdout, ")");
            break;
        default:
            assert(0);
            break;
    }
}

int main(int argc, char *argv[]) {


    if( argc < 2 ) {
        fprintf(stderr, "usage: parse file-name\n");
        exit(1);
    }

    const char *fname = argv[1];

    FILE *file = fopen(fname, "rb");

    if( !file ) {
        fprintf(stderr, "*** fatal. %s: %s \n", fname, strerror(errno));
        exit(-1);
    }


    void *allocator = 0;
    alloc_function_t alloc = example_alloc;
    dealloc_function_t dealloc = example_dealloc;

    struct exp_parser pmem = { 0 };
    struct exp_parser *p = exp_parser_create( &pmem
                                            , file
                                            , file_read_char
                                            , allocator
                                            , alloc
                                            , dealloc );

    struct exp_cell *cell = exp_parse(p, exp_atom("prog", allocator, alloc) );

    dump_cell(0, cell);

__exit:

    exp_parser_destroy(&p);
    fclose(file);

    return 0;
}

