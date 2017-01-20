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

typedef void (*ulisp_parser_err_fn)(void *cc, ulisp_parser_err err, size_t lno, const char *misc);

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
                              , struct stringreader *rd
                              , bool cache );

size_t ustring_length(struct ucell *us);
const char *ustring_cstr(struct ucell *us);

const char *ucell_typename( ucell_type tp );
const char *ulisp_typename( struct ulisp *u, ucell_t *cell );

bool ucell_is_string(ucell_t *s);
ucellp_t ucell_to_string(struct ulisp *, ucellp_t);

integer ucell_intval(struct ucell *us);
object ucell_object(ucell_t *us);

#define nil ((void*)0)
#define isnil(c) ((c) == nil)

#define ornil(v,e) ((v)?(e):nil)

#define QUOTE(u) atom((u), "quote")

#define cstring(u, s) \
ornil((s),umake_stringlike((u), STRING, mk_cstring_reader(pstacktmp(struct cstring_reader), (char*)(s)), true))

#define atom(u, s) \
ornil((s),umake_stringlike((u), ATOM, mk_cstring_reader(pstacktmp(struct cstring_reader), (char*)(s)), true))

#define cstring_(u, s) \
ornil((s),umake_stringlike((u), STRING, mk_cstring_reader(pstacktmp(struct cstring_reader), (char*)(s)), false))

#define atom_(u, s) \
ornil((s),umake_stringlike((u), ATOM, mk_cstring_reader(pstacktmp(struct cstring_reader), (char*)(s)), false))

#define integer(u, i) umake((u), INTEGER, 1, (struct ucell*)(i))
#define cons(u, a, b) umake((u), CONS, 2, (a), (b))
#define quote(u, e) cons((u), QUOTE((u)), cons((u),(e),nil))

#define bind(u,n,what) tuple((u), 2, atom((u), (n)), (what))

#define closure(u, code, n, ...) umake((u), CLOSURE, 1, tuple((u), (n)+1, code, ##__VA_ARGS__))

#define primop(u, op) umake((u), PRIMOP, 1, (op))
#define object(u, obj) umake((u), OBJECT, 1, (obj))


struct ulisp_primop *ucell_primop(ucell_t *e);

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
