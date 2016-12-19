#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// FIXME: move to top-level header

typedef void* (*alloc_function_t)(void*, size_t);
typedef void  (*dealloc_function_t)(void*, void*);

// FIXME: move to header
struct expconf;

struct expconf *expconf_create( void *allocator
                              , alloc_function_t alloc
                              , dealloc_function_t dealloc );

void expconf_destroy( struct expconf **conf );

// FIXME: move to impl.

#include <string.h>

struct expconf {
    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
};


struct expconf *expconf_create( void *allocator
                              , alloc_function_t alloc
                              , dealloc_function_t dealloc ) {

    struct expconf *conf = alloc(allocator, sizeof(struct expconf));

    if( !conf ) {
        return 0;
    }

    memset(conf, 0, sizeof(struct expconf));
    conf->allocator = allocator;
    conf->alloc = alloc;
    conf->dealloc = dealloc;

    return conf;
}

void expconf_destroy( struct expconf **conf ) {
    struct expconf *e = *conf;
    e->dealloc(e->allocator, *conf);
    *conf = 0;
}

// this file

void *__dumb_alloc(void *cc, size_t size) {
    return malloc(size);
}

void __dumb_dealloc(void *cc, void *mem) {
    free(mem);
}


int main(int argc, char **argv) {

    struct expconf *exp = expconf_create( 0
                                        , __dumb_alloc
                                        , __dumb_dealloc);

    assert( exp );

    fprintf(stderr, "expconf allocated\n");

    expconf_destroy(&exp);

    fprintf(stderr, "expconf deallocated\n");

    return 0;
}

