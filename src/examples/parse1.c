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
      LIT_INTEGER
    , NIL
} exp_cell_type;

struct exp_cell {
    exp_cell_type tag;

    union {
        struct exp_cell *expr;
        integer          integer;
    } car;

    struct exp_cell *cdr;
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
                             , void *data
                             , void *allocator
                             , alloc_function_t alloc) {


    size_t cell_size = sizeof(struct exp_cell);
    struct exp_cell *cell = alloc(allocator, cell_size);

    if( !cell ) {
        // FIXME: out of memory
        return exp_nil;
    }

    memset(cell, 0, cell_size);

    switch( t ) {
        case LIT_INTEGER:
            cell->tag = t;
            cell->car.integer = *(integer*)data;
            break;
    }

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

struct exp_cell *exp_parse_expr(struct exp_parser *p) {
    struct exp_token *tok = exp_parser_get_token(p);

    if( !tok ) {
        return exp_nil;
    }

    switch( tok->tag ) {
        case TOK_INTEGER:
            fprintf(stderr, "GOT INT LITERAL %ld\n", tok->v.intval);
            return exp_cell_new(LIT_INTEGER, &tok->v.intval, p->allocator, p->alloc);

        default:
            // TODO: parse error?
            return exp_nil;
    }

    return exp_nil;
}

struct exp_cell *exp_parse( struct exp_parser *parser ) {
    return exp_parse_expr(parser);
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

    struct exp_parser pmem = { 0 };
    struct exp_parser *p = exp_parser_create( &pmem
                                            , file
                                            , file_read_char
                                            , 0
                                            , example_alloc
                                            , example_dealloc );

    struct exp_cell *cell = exp_parse(p);

    fprintf(stderr, "cell %p\n", cell);


__exit:

    exp_parser_destroy(&p);
    fclose(file);

    return 0;
}

