#include "strchunk.h"
#include <string.h>

size_t strchunk_used( struct strchunk *s ) {
    return s->p - s->data;
}

struct strchunk * strchunk_create( void *allocator
                                 , alloc_function_t alloc
                                 ) {

    const size_t chunksize = 4096;
    return strchunk_fixed(alloc(allocator, chunksize), chunksize);
}

struct strchunk * strchunk_fixed( void *mem
                                , size_t mem_size ) {


    if( !mem ) {
        return 0;
    }

    const size_t chunksize = mem_size;
    const size_t ssize = sizeof(struct strchunk);
    const size_t smax = chunksize > ssize ? chunksize - ssize : 0;

    if( mem_size < sizeof(struct strchunk)) {
        return 0;
    }

    struct strchunk *s = mem;

    memset(s, 0, chunksize);

    s->next = 0;
    s->data = &s->buf[0];
    s->p    = s->data;
    s->end  = &s->buf[smax];

    return s;
}

void strchunk_destroy( struct strchunk *chunk
                     , void *allocator
                     , dealloc_function_t dealloc  ) {

    struct strchunk *head = chunk;

    while( head ) {
        struct strchunk *tmp = head;
        head = head->next;
        dealloc(allocator, tmp);
    }
}

bool strchunk_avail( struct strchunk *chunk ) {
    return chunk->p < chunk->end;
}


size_t strchunk_length( struct strchunk *chunk_ ) {
    struct strchunk *chunk = chunk_;
    size_t len = 0;

    for(; chunk; chunk = chunk->next ) {
        len += strchunk_used(chunk);
    }

    return len;
}

unsigned char *strchunk_cstr(struct strchunk *chunk, unsigned char *dst, size_t len) {
    unsigned char *dp = dst;
    unsigned char *de = &dst[len-1];

    for(; dp < de && chunk; chunk = chunk->next ) {
        unsigned char *p = chunk->data;
        unsigned char *pe = chunk->p;
        for(; p < pe && dp < de; p++, dp++ ) {
            *dp = *p;
        }
    }

    *dp = 0;

    return dst;
}

bool strchunk_append_char( struct strchunk *chunk_
                         , unsigned char c
                         , void *allocator
                         , alloc_function_t alloc
                         ) {

    struct strchunk *s = strchunk_avail(chunk_) ? chunk_
                                                : strchunk_create(allocator, alloc);

    if( !s ) {
        return false;
    }

    *s->p = c;
     s->p++;

    return true;
}

