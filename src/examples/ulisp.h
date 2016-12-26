#ifndef __ulisp_h
#define __ulisp_h

#include "ulisp_platform.h"

typedef enum {
     LIST
   , INTEGER
   , ATOM
   , STRING
} ucell_type;

struct ulisp;
struct ucell;


size_t ulisp_size();

struct ulisp *ulisp_create( void *mem
                          , size_t memsize
                          , void *allocator
                          , alloc_function_t alloc
                          , dealloc_function_t dealloc );

void ulisp_destroy( struct ulisp *ulisp );

struct ucell* cons( struct ulisp *l
                  , ucell_type tp
                  , void *car
                  , struct ucell *cdr );

struct ucell* list(struct ulisp *l, ...);

struct ucell* car( struct ucell *cell );
struct ucell* cdr( struct ucell *cell );

#define nil ((void*)0)
#define isnil(c) ((c) == nil)
#define mkinteger(u, iv) cons((u), INTEGER, (void*)iv, nil)

struct ucell_walk_cb {
    void *cc;
    void (*on_list_start)(void *cc);
    void (*on_list_end)(void *cc);
    void (*on_integer)(void *cc, integer data);
    void (*on_nil)(void *cc);
};

void ucell_walk( struct ulisp *ulisp
               , struct ucell *cell
               , struct ucell_walk_cb *cb );

#endif
