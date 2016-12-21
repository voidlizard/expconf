#include "expconf.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "strchunk.h"

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
