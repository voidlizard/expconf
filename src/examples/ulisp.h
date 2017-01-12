#ifndef __ulisp_h
#define __ulisp_h

#include "ulisp_platform.h"
#include "exp_tokenize.h"

typedef enum {
     CONS
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

typedef enum {
    ERR__UNBALANCED_PAREN
  , ERR__INVALID_TOKEN
} ulisp_parser_err;

typedef void (*ulisp_parser_err_fn)(void *cc, ulisp_parser_err err, size_t lno, char *misc);

const char *ulisp_parse_err_str(ulisp_parser_err err);

struct ulisp_parser *ulisp_parser_create( void *mem
                                        , size_t memsize
                                        , read_char_fn readfn
                                        , void *efn_cc
                                        , ulisp_parser_err_fn efn
                                        , void *allocator
                                        , alloc_function_t alloc
                                        , dealloc_function_t dealloc
                                        , struct ulisp *u
                                        );

struct ucell *ulisp_parse( struct ulisp_parser *p, void *reader );
struct ucell *ulisp_parse_top( struct ulisp_parser *p, void *what );

void ulisp_parser_destroy( struct ulisp_parser *p );

struct ucell *umake(struct ulisp *u, ucell_type tp, size_t n, ...);

struct ucell *umake_stringlike( struct ulisp *u
                              , ucell_type tp
                              , struct stringreader *rd );

#define nil ((void*)0)
#define isnil(c) ((c) == nil)

#define setcar(cell, v) (cell)->data[0] = (v)
#define setcdr(cell, v) (cell)->data[1] = (v)
#define car(cell) (cell)->data[0]
#define cdr(cell) (cell)->data[1]

#define ucell_int(cell) ((integer)car((cell)))

#define cstring(u, s) \
umake_stringlike((u), STRING, mk_cstring_reader(pstacktmp(struct cstring_reader), (s)))

#define atom(u, s) \
umake_stringlike((u), ATOM, mk_cstring_reader(pstacktmp(struct cstring_reader), (s)))

#define integer(u, i) umake((u), INTEGER, 1, (struct ucell*)(i))
#define cons(u, a, b) umake((u), CONS, 2, (a), (b))

struct ucell *list(struct ulisp *u, ...);

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
