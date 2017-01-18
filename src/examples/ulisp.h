#ifndef __ulisp_h
#define __ulisp_h

#include "ulisp_types.h"
#include "ulisp_bind.h"
#include "exp_tokenize.h"

size_t ulisp_size();

struct ulisp *ulisp_create( void *mem
                          , size_t memsize
                          , void *allocator
                          , alloc_function_t alloc
                          , dealloc_function_t dealloc );

void ulisp_destroy( struct ulisp *ulisp );

void ulisp_bind(struct ulisp *u, ucell_t *bindlist);

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

void ulisp_eval_top( struct ulisp *u, struct ucell *top );
ucell_t *ulisp_eval_expr( struct ulisp *u, ucell_t *expr );

struct ucell *umake(struct ulisp *u, ucell_type tp, size_t n, ...);

struct ucell *umake_stringlike( struct ulisp *u
                              , ucell_type tp
                              , struct stringreader *rd );

size_t ustring_length(struct ucell *us);
const char *ustring_cstr(struct ucell *us);

integer ucell_intval(struct ucell *us);
void* ucell_tuple_get(ucell_t *tuple, size_t n);

#define nil ((void*)0)
#define isnil(c) ((c) == nil)


#define cstring(u, s) \
umake_stringlike((u), STRING, mk_cstring_reader(pstacktmp(struct cstring_reader), (s)))

#define atom(u, s) \
umake_stringlike((u), ATOM, mk_cstring_reader(pstacktmp(struct cstring_reader), (s)))

#define integer(u, i) umake((u), INTEGER, 1, (struct ucell*)(i))
#define cons(u, a, b) umake((u), CONS, 2, (a), (b))

#define bind(u,n,what) tuple((u), 2, atom((u), (n)), (what))

#define primop(u, op) primop0((u),(op))
#define primopcc(u,op,cc) primop0((u),(op),(cc))
#define primop0(u,op) umake((u), PRIMOP, 2, (op), 0)
#define primop1(u,op,cc) umake((u), PRIMOP, 2, (op), (cc))

struct ucell *list(struct ulisp *u, ...);
struct ucell *tuple(struct ulisp *u, size_t size, ...);

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
