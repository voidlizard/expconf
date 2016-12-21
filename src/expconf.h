#ifndef __expconf_h
#define __expconf_h

#include <stdbool.h>
#include <stddef.h>

#include "allocators.h"


struct expconf;
struct expconf_parser;

struct expconf *expconf_create( void *allocator
                              , alloc_function_t alloc
                              , dealloc_function_t dealloc );

void expconf_destroy( struct expconf **conf );


#endif
