#ifndef __ulisp_parser_h
#define __ulisp_parser_h

#include "ulisp.h"
#include "exp_tokenize.h"

typedef enum {
    ERR__UNBALANCED_PAREN
  , ERR__INVALID_TOKEN
} ulisp_parser_err;

typedef void (*ulisp_parser_err_fn)(void *,ulisp_parser_err,eval_context,const char*);

struct ulisp_parser;
size_t ulisp_parser_size();

const char *ulisp_parse_err_str(ulisp_parser_err err);

struct ulisp_parser *ulisp_parser_create( void *mem
                                        , size_t memsize
                                        , read_char_fn readfn
                                        , void *efn_cc
                                        , ulisp_parser_err_fn efn
                                        , void *allocator
                                        , alloc_function_t alloc
                                        , dealloc_function_t dealloc
                                        , struct ulisp *u
                                        );

struct ucell *ulisp_parse( struct ulisp_parser *p, void *reader );
struct ucell *ulisp_parse_top( struct ulisp_parser *p, void *what );

void ulisp_parser_destroy( struct ulisp_parser *p );

#endif
