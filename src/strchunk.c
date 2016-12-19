#include "strchunk.h"
#include <string.h>

// to miscdata ?
struct strchunk {
    struct strchunk *next;
    unsigned char *sp;
    unsigned char *se;
    unsigned char sdata[0];
};

size_t strchunk_used( struct strchunk *s ) {
    return s->sp - s->sdata;
}

struct strchunk * strchunk_create( void *allocator
                                 , alloc_function_t alloc
                                 ) {

    const size_t chunksize = 4096;
    const size_t ssize = sizeof(struct strchunk);

    struct strchunk *s = alloc(allocator, chunksize);

    if( !s ) {
        return 0;
    }

    memset(s, 0, chunksize);

    s->sp = &s->sdata[0];

    const size_t smax = chunksize > ssize ? chunksize - ssize : 0;

    s->se = &s->sdata[smax];

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
    return chunk->sp < chunk->se;
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
        unsigned char *p = chunk->sdata;
        unsigned char *pe = chunk->sp;
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

    *s->sp = c;
     s->sp++;

    return true;
}

