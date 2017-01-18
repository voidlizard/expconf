#include "ulisp.h"
#include "ulisp_macro.h"

#include <string.h>
#include <stdarg.h>

#include "hash.h"
#include "hashfun_murmur.h"
#include "stringlike.h"

#define setcar(cell, v) (cell)->data[0] = (v)
#define setcdr(cell, v) (cell)->data[1] = (v)
#define car(cell) (cell)->data[0]
#define cdr(cell) (cell)->data[1]

#define ucell_int(cell) ((integer)car((cell)))

#define cstring_tok(u, s) \
umake_stringlike((u), STRING, mk_strchunk_reader(pstacktmp(struct strchunk_reader), (s)))

#define atom_tok(u, s) \
umake_stringlike((u), ATOM, mk_strchunk_reader(pstacktmp(struct strchunk_reader), (s)))

struct ulisp {

    // FIXME: remove obsolete
    struct hash *hstr;

    struct hash *dict;

    // allocator boilerplate
    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
};

struct ucell {
    ucell_type tp;
    struct ucell *data[1];
};

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
#define uprimop_val(e) (((e)->tp == PRIMOP) ? ((struct ulisp_primop*)(e)->data[0]) : 0)

integer ucell_intval(struct ucell *us) {
    return ucell_int(us);
}


static inline void utuple_set(struct ucell *t, size_t i, ucell_t *v) {
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

static inline ucell_t *utuple_get(struct ucell *t, size_t i) {
    struct utuple *tpl = utuple_val(t);
    if( !tpl ) {
        // FIXME: error?
        return nil;
    }
    return i < tpl->len ? tpl->t[i] : nil;
}

void* ucell_tuple_get(ucell_t *tuple, size_t i) {
    return utuple_get(tuple,i);
}

static inline size_t arity(struct ucell *expr) {
    if( isnil(expr) )
        return 0;

    switch( expr->tp ) {
        case PRIMOP:
            return uprimop_val(expr)->arity;

        default:
            return 0;
    }
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

static integer __list_length(ucell_t *e) {
    integer l = 0;
    for(; !isnil(e); e = cdr(e) ) l++;
    return l;
}

static ucell_t *tuple_alloc(struct ulisp *u, size_t n) {
    ucell_t *tpl = umake_nil(u, TUPLE, n+1);

    if( !tpl ) {
        // FIXME: ERROR: out of memory
        return nil;
    }

    struct utuple *ttpl = utuple_val(tpl);

    ttpl->len = n;
    return tpl;
}

static ucell_t *tuplecons(struct ulisp *u, ucell_t *e, bool eval) {

    integer l = __list_length(e);

    ucell_t *tpl = tuple_alloc(u,l);

    integer i = 0;
    ucell_t *ee = e;

    for(; !isnil(ee); ee = cdr(ee), i++ ) {
        utuple_set(tpl, i, eval ? ulisp_eval_expr(u, car(ee)) : car(ee));
    }

    return tpl;
}

struct ucell *tuple(struct ulisp *u, size_t size, ...) {
    ucell_t *tpl = tuple_alloc(u, size);

    va_list ap;
    va_start(ap, size);

    size_t i = 0;
    for(;i < size; i++ ) {
        utuple_set(tpl, i, va_arg(ap, struct ucell*));
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
    return hash_murmur3_32(ustring_cstr(k), ustring_length(k), 0x00BEEF);
}

static bool __pustring_eq(void *a, void *b) {
    struct ucell *k1 = *(struct ucell**)a;
    struct ucell *k2 = *(struct ucell**)b;

    size_t k1len = ustring_length(k1);
    size_t k2len = ustring_length(k2);

    return  k1len  == k2len && 0 == memcmp(ustring_cstr(k1), ustring_cstr(k2), k1len);
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
    struct udict_key *k = b;
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
                              , struct stringreader *rd ) {

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
                          , void *allocator
                          , alloc_function_t alloc
                          , dealloc_function_t dealloc ) {


    if( memsize < ulisp_size() ) {
        return 0;
    }

    struct ulisp *l = mem;
    memset(l, 0, ulisp_size());

    SET_ALLOCATOR(l, allocator, alloc, dealloc);

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

    return l;
}

void ulisp_destroy( struct ulisp *l ) {
    hash_destroy(l->hstr);
    l->dealloc(l->allocator, l->hstr);
    hash_destroy(l->dict);
    l->dealloc(l->allocator, l->dict);
}

static void ucell_walk_rec( struct ulisp *ulisp
                          , struct ucell *cell
                          , struct ucell_walk_cb *cb
                          , struct ucell *parent ) {

    if( cell == nil ) {
        cb->on_nil(cb->cc);
        return;
    }

    switch( cell->tp ) {
        case INTEGER:
            safeappv(cb->on_integer, cb->cc, ucell_int(cell));
            break;

        case CONS:
            safeappv(cb->on_list_start, cb->cc);
            ucell_walk_rec(ulisp, car(cell), cb, cell);
            ucell_walk_rec(ulisp, cdr(cell), cb, cell);
            safeappv(cb->on_list_end, cb->cc);
            break;

        case ATOM:
            safeappv( cb->on_atom
                    , cb->cc
                    , ustring_length(cell)
                    , ustring_cstr(cell) );
            break;

        case STRING:
            safeappv( cb->on_string
                    , cb->cc
                    , ustring_length(cell)
                    , ustring_cstr(cell) );
            break;

        default:
            assert(0);
    }
}


void ucell_walk( struct ulisp *ulisp
               , struct ucell *cell
               , struct ucell_walk_cb *cb ) {
    ucell_walk_rec(ulisp, cell, cb, 0);
}

static void __dict_alter_value( void *cc, void *k_, void *v, bool n ) {
    struct udict_key *k = k_;
    *(struct ucell**)v = *(struct ucell**)cc;
    __udict_val_cpy(v, cc);
}

static void __debug_dump_dict(void *cc, void *k_, void *v_) {
    struct udict_key *k = k_;
    struct ucell **v = v_;
    fprintf(stderr, "dict entry %p %p\n", k->p, *v);
}

static void ulisp_bind_one(struct ulisp *u, ucell_t *bind) {

    struct utuple *tpl = utuple_val(bind);

    if( !tpl ) {
        // FIXME: error notification
        fprintf(stderr, "*** error (init): bind must be a tuple\n");
        assert(0);
    }

    struct udict_key ktmp = { 0 };
    struct udict_key *k = udict_key_init(&ktmp, utuple_get(bind, 0));
    ucell_t *value = utuple_get(bind, 1);

    if( !k ) {
        // FIXME: error notification
        fprintf(stderr, "*** error (init): bind key must be a string\n");
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

// parser

struct ulisp_parser {
    read_char_fn readfn;

    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
    struct ulisp *u;

    struct {
        struct exp_token   mem;
        struct exp_token  *curr;
        struct exp_token  *back;
    } token;

    struct {
        size_t lno;
        void *cc;
        bool eof;
    } rdr;

    struct exp_tokenizer *tokenizer;
    void *on_err_cc;
    ulisp_parser_err_fn on_err;

};

size_t ulisp_parser_size() {
    return sizeof(struct ulisp_parser);
}

struct ulisp_parser *ulisp_parser_create( void *mem
                                        , size_t memsize
                                        , read_char_fn rfn
                                        , void *efn_cc
                                        , ulisp_parser_err_fn efn
                                        , void *allocator
                                        , alloc_function_t alloc
                                        , dealloc_function_t dealloc
                                        , struct ulisp *u
                                        ) {

    if( memsize < ulisp_parser_size() ) {
        return 0;
    }

    struct ulisp_parser *p = mem;
    *p = (struct ulisp_parser) { .readfn = rfn
                               , .allocator = allocator
                               , .alloc = alloc
                               , .dealloc = dealloc
                               , .u = u
                               , .token = { 0 }
                               , .tokenizer = 0
                               , .rdr = { 0 }
                               , .on_err_cc = efn_cc
                               , .on_err = efn
                               };


    p->rdr.lno = 1;
    p->rdr.eof = false;

    return p;
}

static inline void destroy_tokenizer( struct ulisp_parser *p ) {
    if( p->tokenizer ) {
        exp_tokenizer_destroy(&p->tokenizer);
    }
}

void ulisp_parser_destroy( struct ulisp_parser *p ) {
    destroy_tokenizer(p);
}

static struct exp_token *token_get( struct ulisp_parser *p  ) {

    if( p->token.back ) {
        struct exp_token *tmp = p->token.back;
        p->token.back = 0;
        return tmp;
    }

    p->token.curr = exp_tokenizer_next(p->tokenizer, &p->token.mem);
    return p->token.curr;
}

static void token_unget( struct ulisp_parser *p ) {
    p->token.back = p->token.curr;
}

static bool __lno_reader_wrapper( void *cc, unsigned char *c ) {
    struct ulisp_parser *p = cc;
    unsigned char nc = 0;
    bool ok = p->readfn(p->rdr.cc, &nc);

    if( !ok ) {
        p->rdr.eof = true;
        return false;
    }

    *c = nc;

    if( nc == '\n' ) {
        p->rdr.lno++;
    }

    return ok;
}

static void reset_tokenizer( struct ulisp_parser *p, void *reader ) {

    destroy_tokenizer(p);

    p->tokenizer = exp_tokenizer_create( p->allocator
                                       , p->alloc
                                       , p->dealloc
                                       , p
                                       , __lno_reader_wrapper );

}

static struct ucell *parse_expr( struct ulisp_parser *p, struct ucell *top );

static struct ucell *parse_list( struct ulisp_parser *p ) {
    struct ulisp *u = p->u;
    struct exp_token *tok = token_get(p);

    if( !tok ) {
        p->on_err(p->on_err_cc, ERR__UNBALANCED_PAREN, p->rdr.lno, "");
        return nil;
    }

    if( tok->tag == TOK_CPAREN ) {
        return nil;
    }

    token_unget(p);

    struct ucell *car = parse_expr(p, nil);
    return cons(u, car, parse_list(p));
}

static struct ucell *parse_expr( struct ulisp_parser *p, struct ucell *top ) {

    do {

        struct exp_token *tok = token_get(p);
        struct ulisp *u = p->u;

        if( !tok ) {
            return nil;
        }

        switch( tok->tag ) {

            case TOK_NEWLINE:
                continue;

            case TOK_INTEGER:
                return integer(u, tok->v.intval);

            case TOK_STRING:
                return cstring_tok(u, tok->v.strval);

            case TOK_ATOM:
                return atom_tok(u, tok->v.atom);

            case TOK_OPAREN:
                return parse_list(p);

            case TOK_CPAREN:
                p->on_err(p->on_err_cc, ERR__UNBALANCED_PAREN, p->rdr.lno, "");
                return nil;

            case TOK_ERROR:
                p->on_err(p->on_err_cc, ERR__INVALID_TOKEN, p->rdr.lno, "");
                return nil;

            case TOK_COMMA:
                return atom(u, ",");

            case TOK_SEMI:
                return atom(u, ";");

            default:
                assert(0);
        }

    } while(1);

    return nil;
}

static struct ucell *ulisp_parse_(struct ulisp_parser *p) {
    return parse_expr(p, list(p->u, nil));
}

struct ucell *ulisp_parse( struct ulisp_parser *p, void *what ) {
    p->rdr.cc = what;
    reset_tokenizer(p, what);
    return ulisp_parse_(p);
}

struct ucell *ulisp_parse_top( struct ulisp_parser *p, void *what ) {

    p->rdr.cc = what;
    reset_tokenizer(p, what);

    struct ulisp *u = p->u;
    struct ucell *head = nil;
    struct ucell *prev = nil;

    while( !p->rdr.eof ) {
        struct ucell *expr = ulisp_parse_(p);

        if( !expr || p->rdr.eof ) {
            break;
        }

        if( isnil(prev) ) {
            head = prev = cons(u, expr, nil);
        } else {
            setcdr(prev, cons(u, expr, nil));
            prev = cdr(prev);
        }
    }

    return head;
}



ucell_t *apply_tuple( struct ulisp *u, ucell_t *tuple ) {

    ucell_t *expr = utuple_get(tuple, 0);

    if( isnil(tuple) || isnil(expr) ) {
        // FIXME: error
        fprintf(stderr, "*** error (runtime): can't apply nil\n");
        return nil;
    }

    size_t ar = utuple_len(u, tuple)-1;
    if( ar != arity(expr) ) {
        // FIXME: error
        fprintf(stderr, "*** error (runtime): arity mismatch %ld ~ %ld\n", ar, arity(expr));
        return nil;
    }

    if( expr->tp == PRIMOP ) {
        GENERATE_PRIMOP_CALL(ULISP_PRIMOP_MAX_ARITY, tuple);
        return nil;
    }

    return expr;
}


ucell_t *ulisp_eval_expr( struct ulisp *u, ucell_t *expr ) {

    if( isnil(expr) )
        return nil;

    switch( expr->tp ) {
        case ATOM: {
            ucell_t *e = udict_lookup(u, expr);
            if( isnil(e) ) {
                // FIXME: error handling
                fprintf(stderr, "*** error (runtime): unbound symbol %s\n", ustring_cstr(expr));
                return nil;
            }
            return e;
        }

        case INTEGER:
            return expr;

        case STRING:
            return expr;

        case CONS:
            return apply_tuple(u, tuplecons(u, expr, true));

        default:
            assert(0);
    }

    return nil;
}

void ulisp_eval_top( struct ulisp *u, struct ucell *top ) {

    ucell_t *expr = top;

    for( ; expr; expr = cdr(expr) ) {

        if( isnil(car(expr)) || car(expr)->tp != CONS ) {
            // FIXME: normal error notification
            // FIXME: stop on error?
            // FIXME: line number (???)
            fprintf(stderr, "*** error (eval): invalid top-level construct as line ()\n");
            continue;
        }

        (void)ulisp_eval_expr(u, car(expr));
    }
}

const char *ulisp_parse_err_str(ulisp_parser_err err) {
    switch(err) {
        case ERR__UNBALANCED_PAREN:
            return "unbalanced parens";
        case ERR__INVALID_TOKEN:
            return "invalid token format (string, number, atom?)";
        default:
            return "unknown parse error";
    }
}

