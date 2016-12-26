#ifndef __allocators_h
#define __allocators_h

#include <stddef.h>

// FIXME: move to top-level header (miscdata?)
typedef void* (*alloc_function_t)(void*, size_t);
typedef void  (*dealloc_function_t)(void*, void*);

#define SET_ALLOCATOR(to, allocator, alloc, dealloc)\
    do { (to)->allocator = (allocator);\
         (to)->alloc = (alloc);\
         (to)->dealloc = (dealloc);\
    } while(0)

#endif
