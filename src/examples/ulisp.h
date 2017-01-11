#ifndef __ulisp_h
#define __ulisp_h

#include "ulisp_platform.h"
#include "exp_tokenize.h"

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

struct ulisp_parser;
size_t ulisp_parser_size();

struct ulisp_parser *ulisp_parser_create( void *mem
                                        , size_t memsize
                                        , read_char_fn readfn
                                        , void *allocator
                                        , alloc_function_t alloc
                                        , dealloc_function_t dealloc
                                        , struct ulisp *u );

struct ucell *ulisp_parse( struct ulisp_parser *p, void *reader );

void ulisp_parser_destroy( struct ulisp_parser *p );

struct ucell* cons( struct ulisp *l
                  , ucell_type tp
                  , void *car
                  , struct ucell *cdr );

struct ucell* list(struct ulisp *l, ...);

struct ucell* string(struct ulisp *l, struct stringreader *sl);
struct ucell* atom(struct ulisp *l, struct stringreader *sl);

struct ucell* car( struct ucell *cell );
struct ucell* cdr( struct ucell *cell );

#define nil ((void*)0)
#define isnil(c) ((c) == nil)
#define mkinteger(u, iv) cons((u), INTEGER, (void*)iv, nil)

static inline struct ucell* mkcstring(struct ulisp *u, void *cs) {
    struct cstring_reader csrd;
    return string(u, mk_cstring_reader(&csrd, cs));
}

static inline struct ucell* mkatom(struct ulisp *u, void *cs) {
    struct cstring_reader csrd;
    return atom(u, mk_cstring_reader(&csrd, cs));
}

struct ucell_walk_cb {
    void *cc;
    void (*on_list_start)(void *cc);
    void (*on_list_end)(void *cc);
    void (*on_integer)(void *cc, integer data);
    void (*on_string)(void *cc, size_t l, const char *cstr);
    void (*on_atom)(void *cc, size_t l, const char *cstr);
    void (*on_nil)(void *cc);
};

void ucell_walk( struct ulisp *ulisp
               , struct ucell *cell
               , struct ucell_walk_cb *cb );

#endif
