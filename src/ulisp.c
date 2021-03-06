#include "ulisp.h"
#include "ulisp_macro.h"

#include <string.h>
#include <stdarg.h>

#include "miscdata.h"
#include "hash.h"
#include "hashfun_murmur.h"
#include "stringlike.h"

#include "ulisp_internal.h"

#define ucell_int(cell) ((integer)car((cell)))

struct ustring {
    size_t len;
    char data[0];
};

struct utuple {
    integer len;
    ucell_t *t[0];
};

struct udict_key {
    struct ucell *p;
};

#define utuple_val(e) (((e)->tp == TUPLE) ? ((utuple_t*)(e)->data) : 0)

static void ulisp_bind_one(struct ulisp *u, ucell_t *bind);
static inline ucell_t *udict_lookup(struct ulisp *u, ucell_t *expr );
static inline int arity(struct ulisp *u, struct ucell *expr);

static inline void eval_error( struct ulisp *u
                             , ulisp_eval_err err
                             , ucell_t *e
                             , const char *msg ) {

    ucell_t *wtf = ulisp_eval_expr(u, atom(u, "__UNIT__"));

    char tmp[ULISP_MAX_ERROR_MSG];
    snprintf(tmp, sizeof(tmp), "%s:%d", ustring_cstr(wtf), e->lno);
    safecall(unit, u->error_fn, u->error_cc, err, tmp, msg);

    if( u->abrt ) {
        longjmp( *u->abrt, 1);
    }

    exit(-1);
}

static inline void eval_error_application( struct ulisp *u
                                         , ucell_t *expr
                                         , ucell_type *tp ) {


    char tmp[256];
    snprintf( tmp
            , sizeof(tmp)
            , "expected applicable, got %s"
            , tp ? ucell_typename(*tp) : "#nil");

    eval_error(u, ERR__EVAL_TYPE, expr, tmp);
}

static inline void eval_error_typecheck( struct ulisp *u
                                       , ucell_t *ctx
                                       , ucell_type tp
                                       , ucell_t *e ) {

    char tmp[256];
    snprintf( tmp
            , sizeof(tmp)
            , "expected type %s, got %s"
            , ucell_typename(tp)
            , ulisp_typename(u, e));

    eval_error(u, ERR__EVAL_TYPE, ctx, tmp);
}

static inline void eval_error_arity( struct ulisp *u
                                   , ucell_t *expr
                                   , int ar1
                                   , int ar2
                                   ) {

    char tmp[256];
    snprintf(tmp, sizeof(tmp), "expected arity %d, got %d", ar1, ar2);
    eval_error(u, ERR__EVAL_ARITY, expr, tmp);
}

static inline void eval_error_unbound( struct ulisp *u
                                     , ucell_t *expr ) {

    char tmp[256];
    snprintf(tmp, sizeof(tmp), "%s", ustring_cstr(expr));
    eval_error(u, ERR__EVAL_UNBOUND, expr, tmp);
}

static inline void eval_error_toplevel_syntax( struct ulisp *u
                                             , ucell_t *expr) {

    char tmp[256];
    snprintf(tmp, sizeof(tmp), "expression is not allowed at top-level");
    eval_error(u, ERR__EVAL_TYPE, expr, tmp);
}

integer ucell_intval(struct ucell *us) {
    return ucell_int(us);
}

object ucell_object(ucell_t *us) {
    return car(us);
}

struct ulisp_primop *ucell_primop(ucell_t *e) {
    return (!isnil(e) && ((e->tp == PRIMOP) || (e->tp == PRIMOP_LIST))) ? (void*)e->data[0] : 0;
}

static struct ucell *ucell_alloc(struct ulisp *u, size_t bytes) {

    if( bytes < sizeof(struct ucell) )
        return 0;

    return u->alloc(u->allocator, bytes);
}

static struct ucell *umake_nil(struct ulisp *u, ucell_type tp, size_t n) {

    if( n < 1 )
        return nil;

    const size_t ms = sizeof(struct ucell) + (n-1)*sizeof(struct ucell*);
    struct ucell *cell = ucell_alloc(u, ms);

    if( !cell )
        return nil;

    cell->tp = tp;
    cell->lno = 0;

    return cell;
}

struct ucell *umake(struct ulisp *u, ucell_type tp, size_t n, ...) {

    struct ucell *cell = umake_nil(u, tp, n);

    if( isnil(cell) )
        return nil;

    va_list ap;
    va_start(ap, n);

    size_t i = 0;
    for(; i < n; i++ ) {
        cell->data[i] = va_arg(ap, struct ucell*);
    }

    va_end(ap);

    return cell;
}


struct ucell *list(struct ulisp *u, ...) {
    va_list ap;
    va_start(ap, u);

    struct ucell *head = nil;
    struct ucell *prev = nil;

    size_t i = 0;
    for(;i < ULISP_LIST_LIT_MAX; i++ ) {
        struct ucell *c = va_arg(ap, struct ucell*);
        if( isnil(c) ) {
            break;
        }

        if( prev ) {
            setcdr(prev, cons(u, c, nil));
            prev = cdr(prev);
        } else {
            head = prev = cons(u, c, nil);
        }
    }

    va_end(ap);

    return head;
}

static inline size_t utuple_len(struct ulisp *u, struct ucell *t) {
    struct utuple *tpl = utuple_val(t);
    if( !tpl ) {
        return 0;
    }
    return tpl->len;
}

static inline void utuple_set(struct ulisp *u, struct ucell *t, size_t i, ucell_t *v) {
    struct utuple *tpl = utuple_val(t);

    if( !tpl ) {
        // FIXME: error?
        assert(0);
        return;
    }

    if( i < tpl->len ) {
        tpl->t[i] = v;
    } else {
        // FIXME: error?
        assert(0);
    }
}

static inline ucell_t *utuple_get(struct ulisp *u, struct ucell *t, size_t i) {
    struct utuple *tpl = utuple_val(t);
    if( !tpl ) {
        // FIXME: error?
        return nil;
    }
    return i < tpl->len ? tpl->t[i] : nil;
}

static ucell_t *tuple_alloc(struct ulisp *u, size_t n) {
    ucell_t *tpl = umake_nil(u, TUPLE, n+1);

    if( !tpl ) {
        eval_error(u, ERR__EVAL_OOM, 0, "");
        assert(0);
    }

    struct utuple *ttpl = utuple_val(tpl);

    ttpl->len = n;
    return tpl;
}

struct ucell *tuple(struct ulisp *u, size_t size, ...) {
    ucell_t *tpl = tuple_alloc(u, size);

    va_list ap;
    va_start(ap, size);

    size_t i = 0;
    for(;i < size; i++ ) {
        utuple_set(u, tpl, i, va_arg(ap, struct ucell*));
    }

    va_end(ap);

    return tpl;
}

size_t ustring_length(struct ucell *us) {

    if( isnil(us) )
        return 0;

    if( us->tp == STRING || us->tp == ATOM ) {
        return ((struct ustring*)us->data)->len;
    }

    return 0;
}

const char *ustring_cstr(struct ucell *us) {

    if( isnil(us) )
        return 0;

    if( us->tp == STRING || us->tp == ATOM ) {
        return ((struct ustring*)us->data)->data;
    }
    return 0;
}

static uint32_t __pustring_hash(void *a) {
    struct ucell *k = *(struct ucell**)a;
    return hash_murmur3_32((unsigned char*)ustring_cstr(k), ustring_length(k), k->tp ^ 0xDEADBEEF);
}

static bool __pustring_eq(void *a, void *b) {
    struct ucell *k1 = *(struct ucell**)a;
    struct ucell *k2 = *(struct ucell**)b;

    size_t k1len = ustring_length(k1);
    size_t k2len = ustring_length(k2);

    return  k1->tp == k2->tp
         && k1len  == k2len
         && 0 == memcmp(ustring_cstr(k1), ustring_cstr(k2), k1len);
}

void __pustring_cpy(void *a, void *b) {
    *(struct ucell**)a = *(struct ucell**)b;
}

static uint32_t __udict_key_hash(void *k_) {
    struct udict_key *k = k_;
    return (uint32_t)(size_t)k->p;
}

static bool __udict_key_cmp(void *a, void *b) {
    struct udict_key *k1 = a;
    struct udict_key *k2 = b;
    return k1->p == k2->p;
}

static void __udict_key_cpy(void *a, void *b) {
    memcpy(a, b, sizeof(struct udict_key));
}

static void __udict_val_cpy(void *a, void *b) {
    *(struct ucell**)a = *(struct ucell**)b;
}

static struct udict_key *udict_key_init( struct udict_key *mem, ucell_t *expr ) {
    const char *s = ustring_cstr(expr);

    if( !s )
        return 0;

    mem->p = expr;
    return mem;
}

static inline ucell_t *udict_lookup(struct ulisp *u, ucell_t *expr ) {
    void *r = hash_get(u->dict, udict_key_init(pstacktmp(struct udict_key), expr));
    return  r ? *(ucell_t**)r : nil;
}

struct ucell *umake_stringlike( struct ulisp *u
                              , ucell_type tp
                              , struct stringreader *rd
                              , bool cache ) {

    size_t size = sizeof(struct ucell)
                + sizeof(struct ustring)
                + rd->length(rd->cs)
                + 1;

    struct ucell *cell = ucell_alloc(u, size);

    if( !cell ) {
        return 0;
    }

    cell->tp = tp;

    struct ustring *us = (void*)&cell->data[0];
    us->len = rd->length(rd->cs);
    memset(us->data, 0, us->len+1);

    int chr = 0;
    size_t i = 0;
    for( ;rd->readchar(rd->cs, &chr); i++ ) {
        us->data[i] = (char)chr;
    }

    if( !cache ) {
        return cell;
    }

    struct ucell **r = hash_get(u->hstr, &cell);

    if( r ) {
        u->dealloc(u->allocator, cell);
        return *r;
    }

    if( !hash_add(u->hstr, &cell, &cell) ) {
        u->dealloc(u->allocator, cell);
        return 0;
    }

    return cell;
}

size_t ulisp_size() {
    return sizeof(struct ulisp);
}

struct ulisp *ulisp_create( void *mem
                          , size_t memsize
                          , jmp_buf *abrt
                          , void *err_cc
                          , ulisp_on_eval_error efn
                          , void *allocator
                          , alloc_function_t alloc
                          , dealloc_function_t dealloc ) {


    if( memsize < ulisp_size() ) {
        return 0;
    }

    struct ulisp *l = mem;
    memset(l, 0, ulisp_size());

    SET_ALLOCATOR(l, allocator, alloc, dealloc);

    l->abrt = abrt;
    l->error_cc = err_cc;
    l->error_fn = efn;

    const size_t BKT = 64;
    const size_t MINITEMS = 128;
    const size_t hsz = hash_minimal_mem_size( BKT
                                            , MINITEMS
                                            , sizeof(struct ustring*)
                                            , sizeof(struct ustring*) );


    void *hmem = l->alloc(l->allocator, hsz);

    if( !hmem ) {
        return 0;
    }

    l->hstr = hash_create( hsz
                         , hmem
                         , sizeof(struct ustring*)
                         , sizeof(struct ustring*)
                         , BKT
                         , __pustring_hash
                         , __pustring_eq
                         , __pustring_cpy
                         , __pustring_cpy
                         , l->allocator
                         , l->alloc
                         , l->dealloc );


    const size_t dsz = hash_minimal_mem_size( BKT
                                            , MINITEMS
                                            , sizeof(struct udict_key)
                                            , sizeof(struct ucell*) );


    void *dmem = l->alloc(l->allocator, dsz);

    if( !dmem ) {
        return 0;
    }

    l->dict = hash_create( dsz
                         , dmem
                         , sizeof(struct udict_key)
                         , sizeof(struct ucell*)
                         , BKT
                         , __udict_key_hash
                         , __udict_key_cmp
                         , __udict_key_cpy
                         , __udict_val_cpy
                         , l->allocator
                         , l->alloc
                         , l->dealloc );


    ulisp_bind_one(l, bind(l, "__UNIT__", cstring(l, "<unit>")));
    return l;
}

void ulisp_destroy( struct ulisp *l ) {
    hash_destroy(l->hstr);
    l->dealloc(l->allocator, l->hstr);
    hash_destroy(l->dict);
    l->dealloc(l->allocator, l->dict);
}

static void __dict_alter_value( void *cc, void *k_, void *v, bool n ) {
    *(struct ucell**)v = *(struct ucell**)cc;
    __udict_val_cpy(v, cc);
}

static void ulisp_bind_one(struct ulisp *u, ucell_t *bind) {

    struct utuple *tpl = utuple_val(bind);

    if( !tpl ) {
        eval_error(u, ERR__EVAL_TYPE, bind, "bind must be a tuple(string, expression)");
        assert(0);
    }

    struct udict_key ktmp = { 0 };
    struct udict_key *k = udict_key_init(&ktmp, utuple_get(u, bind, 0));
    ucell_t *value = utuple_get(u, bind, 1);

    if( !k ) {
        eval_error(u, ERR__EVAL_TYPE, bind, "bind must be a tuple(string, expression)");
        assert(0);
    }

    if( !value ) {
        return;
    }

    hash_alter(u->dict, true, k, &value, __dict_alter_value);
}

void ulisp_bind(struct ulisp *u, ucell_t *bindlist) {
    ucell_t *e = bindlist;
    for(; e; e = cdr(e) ) ulisp_bind_one(u, car(e));
}

static inline int arity(struct ulisp *u, struct ucell *expr) {
    if( isnil(expr) )
        return 0;

    switch( expr->tp ) {
        case PRIMOP: {
            struct ulisp_primop *op = ucell_primop(expr);
            return op ? op->arity : 0;
        }

        case PRIMOP_LIST: {
            struct ulisp_primop *op = ucell_primop(expr);
            return op ? op->arity : 0;
        }

        case CLOSURE: {
            ucell_t *tpl = car(expr);
            // FIXME: error handle?
            int ar = arity(u, utuple_get(u, tpl, 0));
            int free = utuple_len(u, tpl)-1;
            return ar - free;
        }

        default:
            return 0;
    }
}

static inline void ulisp_expr_typecheck( struct ulisp *u, ucell_type tp, ucell_t *expr ) {

    // NIL ~> any type?
    if( isnil(expr) )
        return;

    if( tp == ANY ) {
        return;
    }

    if( tp == expr->tp ) {
        return;
    }

    eval_error_typecheck(u, expr, tp, expr);
    assert(0);
}

static inline ucell_t *apply_list( struct ulisp *u, ucell_t *expr) {

    if( car(expr) == QUOTE(u) ) {
        return car(cdr(expr));
    }

    ucell_t *appl = ulisp_eval_expr(u, car(expr));
    ucell_t *args = cdr(expr);

    if( isnil(appl) ) {
        eval_error_application(u, expr, 0);
        assert(0);
    }

    if( appl->tp != CLOSURE ) {
        if( !isnil(args) ) {
            eval_error_application(u, expr, &appl->tp);
            assert(0);
        }
        return appl;
    }

    // args  - arguments list
    // car(appl) - call, callee + free vars

    ucell_t *tuple = car(appl);

    ucell_t *fun = utuple_get(u, tuple, 0);

    int  ar = arity(u, fun);
    int  freevars = utuple_len(u, tuple) - 1;

    ucell_t *call = tuple_alloc(u, ar+1);

    utuple_set(u, call, 0, fun);

    int i = 1, j = 0;
    for(; i <= freevars; i++, j++ ) {
        utuple_set(u, call, i, utuple_get(u, tuple, i));
    }

    ucell_t *arg = args;
    int ar1 = i-1 + (isnil(arg) ? 0 : 1);

    if( fun->tp == PRIMOP ) {

        for(; i <= ar && !isnil(arg); arg = cdr(arg), i++ ) {
            utuple_set(u, call, i, ulisp_eval_expr(u, car(arg)));
        }

        if( ar1 != ar ) {
            eval_error_arity(u, appl, ar, ar1);
            assert(0);
        }

        int j = 1;
        struct ulisp_primop *op = ucell_primop(fun);
        for(; j <= ar; j++ ) {
            ulisp_expr_typecheck(u, op->argtp[j-1], utuple_get(u, call, j));
        }

        GENERATE_PRIMOP_CALL(ULISP_PRIMOP_MAX_ARITY, call);
    } else if( fun->tp == PRIMOP_LIST ) {
        if( ar != freevars + 1) {
            eval_error_arity(u, appl, freevars+1, ar);
            assert(0);
        }
        ucell_t *result = nil;
        for(; !isnil(arg); arg = cdr(arg), i++ ) {
            result = cons(u, ulisp_eval_expr(u, car(arg)), result);
        }
        ucell_t *ev = result;
        ucell_t *to = nil;
        for(; !isnil(ev); ev = cdr(ev) ) {
            to = cons(u, car(ev), to);
        }
        utuple_set(u, call, freevars+1, to);
        GENERATE_PRIMOP_CALL(ULISP_PRIMOP_MAX_ARITY, call);
        assert(0);
    }

    eval_error_application(u, expr, &fun->tp);
    assert(0);
}

ucell_t *ulisp_eval_expr( struct ulisp *u, ucell_t *expr ) {

    if( isnil(expr) )
        return nil;

    switch( expr->tp ) {
        case ATOM: {
            ucell_t *e = udict_lookup(u, expr);
            if( isnil(e) ) {
                eval_error_unbound(u, expr);
                assert(0);
            }
            return e;
        }

        case INTEGER:
            return expr;

        case STRING:
            return expr;

        case CONS:
            return apply_list(u, expr);

        default:
            assert(0);
    }

    return nil;
}

void ulisp_eval_top( struct ulisp *u, struct ucell *top ) {

    ucell_t *expr = top;

    for( ; expr; expr = cdr(expr) ) {

        if( isnil(car(expr)) || car(expr)->tp != CONS ) {
            eval_error_toplevel_syntax(u, expr);
            assert(0);
        }

        (void)ulisp_eval_expr(u, car(expr));
    }
}

const char *ulisp_eval_err_str(ulisp_eval_err err) {
    switch(err) {
        case ERR__EVAL_TYPE:
            return "type";

        case ERR__EVAL_ARITY:
            return "bad arity";

        case ERR__EVAL_OOM:
            return "out of memory";

        case ERR__EVAL_UNBOUND:
            return "symbol not bound";

        case ERR__EVAL_INTERNAL:
            return "internal";
    }

    return "unknown";
}



const char *ucell_typename( ucell_type tp ) {
    switch( tp ) {
        case UNIT: return "UNIT";
        case CONS: return "CONS";
        case INTEGER: return "INTEGER";
        case ATOM: return "ATOM";
        case STRING: return "STRING";
        case TUPLE: return "TUPLE";
        case PRIMOP: return "PRIMOP";
        case PRIMOP_LIST: return "PRIMOP";
        case CLOSURE: return "CLOSURE";
        case OBJECT: return "OBJECT";
        case ANY: return "ANY";
    }

    assert(0);
}

const char *ulisp_typename( struct ulisp *u, ucell_t *cell ) {
    if( isnil(cell) ) {
        return "NIL";
    }

    return ucell_typename(cell->tp);
}

bool ucell_is_string(ucell_t *s) {
    if( isnil(s) )
        return false;

    return s->tp == ATOM || s->tp == STRING;

}
