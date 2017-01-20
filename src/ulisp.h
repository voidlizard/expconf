#ifndef __ulisp_h
#define __ulisp_h

#include "ulisp_types.h"
#include "ulisp_bind.h"

typedef enum {
      ERR__EVAL_TYPE
    , ERR__EVAL_ARITY
    , ERR__EVAL_OOM
    , ERR__EVAL_UNBOUND
    , ERR__EVAL_INTERNAL
} ulisp_eval_err;

size_t ulisp_size();

typedef void (*ulisp_on_eval_error)(void*,ulisp_eval_err,eval_context,const char*);

struct ulisp *ulisp_create( void *mem
                          , size_t memsize
                          , jmp_buf *abrt
                          , void *err_cc
                          , ulisp_on_eval_error
                          , void *allocator
                          , alloc_function_t alloc
                          , dealloc_function_t dealloc );

void ulisp_destroy( struct ulisp *ulisp );

void ulisp_bind(struct ulisp *u, ucell_t *bindlist);


void ulisp_eval_top( struct ulisp *u, struct ucell *top );
ucell_t *ulisp_eval_expr( struct ulisp *u, ucell_t *expr );
const char *ulisp_eval_err_str(ulisp_eval_err err);

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
