#ifndef __expconf_h
#define __expconf_h

#include <stdbool.h>
#include <stddef.h>

#include "allocators.h"

typedef bool   (*read_char_t)(void*, unsigned char *);

// FIXME: move to header

#define EXPCONF_LIM_MAX_ATOM 256

typedef size_t expconf_integer;

typedef enum {
     ERR_NO_ERROR
   , ERR_TOK_UNKNOWN
   , ERR_TOK_BAD_ATOM
   , ERR_TOK_BAD_NUMBER
   , ERR_TOK_BAD_STRING
   , ERR_TOK_STRING_TOO_LARGE
} expconf_parser_error;

struct expconf;
struct expconf_parser;

struct expconf *expconf_create( void *allocator
                              , alloc_function_t alloc
                              , dealloc_function_t dealloc );

void expconf_destroy( struct expconf **conf );

struct expconf_parser * expconf_parser_create( void *allocator
                                             , alloc_function_t alloc
                                             , dealloc_function_t dealloc );


void expconf_parser_destroy( struct expconf_parser ** );

void expconf_set_parser_error( struct expconf_parser *p, expconf_parser_error err);

size_t expconf_parser_error_num(struct expconf_parser*);

typedef enum {
     EXPCONF_TOKEN_ATOM
   , EXPCONF_TOKEN_STRING
   , EXPCONF_TOKEN_INTEGER
   , EXPCONF_TOKEN_OPAREN
   , EXPCONF_TOKEN_CPAREN
} expconf_token_tag;

struct expconf_token {
    expconf_token_tag tag;
    union {
        expconf_integer ei;
        struct strchunk *chunk;
    } val;
};

typedef void (*on_token_t)(void *cc, struct expconf_token *);

const char *expconf_token_tag_str(expconf_token_tag tag);

void expconf_tokenize( struct expconf_parser *p
                     , void *reader
                     , read_char_t readfn
                     , void *tok_cc
                     , on_token_t on_token
                     );


#endif
