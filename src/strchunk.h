#ifndef __strchunk_h
#define __strchunk_h

#include <stdbool.h>
#include "allocators.h"

struct strchunk;

size_t strchunk_used( struct strchunk *s );

struct strchunk * strchunk_create( void *allocator
                                 , alloc_function_t alloc
                                 );

void strchunk_destroy( struct strchunk *chunk
                     , void *allocator
                     , dealloc_function_t dealloc  );

size_t strchunk_length( struct strchunk *chunk_ );

unsigned char *strchunk_cstr(struct strchunk *, unsigned char *, size_t);

bool strchunk_append_char( struct strchunk *chunk_
                         , unsigned char c
                         , void *allocator
                         , alloc_function_t alloc
                         );

#endif
