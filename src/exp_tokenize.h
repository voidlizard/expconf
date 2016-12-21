#ifndef __exp_tokenize_h
#define __exp_tokenize_h

#include <stdbool.h>
#include <stddef.h>

#include "allocators.h"
#include "strchunk.h"

typedef bool   (*read_char_fn)(void*, unsigned char *);

struct exp_tokenizer;

struct exp_tokenizer *exp_tokenizer_create( void *allocator
                                          , alloc_function_t  alloc
                                          , dealloc_function_t dealloc
                                          , void *reader
                                          , read_char_fn readchar
                                          );


void exp_tokenizer_destroy( struct exp_tokenizer** );

typedef enum {
    TOK_ERROR
  , TOK_OPAREN
  , TOK_CPAREN
} exp_token_tag;

struct exp_token {
    exp_token_tag tag;
};

bool exp_tokenizer_next( struct exp_tokenizer *t
                       , struct exp_token *tok );


const char *exp_token_tag_name(exp_token_tag);

#endif
