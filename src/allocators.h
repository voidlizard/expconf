#ifndef __allocators_h
#define __allocators_h

#include <stddef.h>

// FIXME: move to top-level header (miscdata?)
typedef void* (*alloc_function_t)(void*, size_t);
typedef void  (*dealloc_function_t)(void*, void*);


#endif
