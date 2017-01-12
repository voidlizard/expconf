#include "ulisp.h"

#include <string.h>
#include <stdarg.h>

#include "hash.h"
#include "hashfun_murmur.h"
#include "stringlike.h"

#define cstring_tok(u, s) \
umake_stringlike((u), STRING, mk_strchunk_reader(pstacktmp(struct strchunk_reader), (s)))

#define atom_tok(u, s) \
umake_stringlike((u), ATOM, mk_strchunk_reader(pstacktmp(struct strchunk_reader), (s)))

struct ulisp {

    struct hash *hstr;

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

#define nil ((void*)0)


static struct ucell *ucell_alloc(struct ulisp *u, size_t bytes) {

    if( bytes < sizeof(struct ucell) )
        return 0;

    return u->alloc(u->allocator, bytes);
}

struct ucell *umake(struct ulisp *u, ucell_type tp, size_t n, ...) {

    if( n < 1 )
        return 0;

    const size_t ms = sizeof(struct ucell) + (n-1)*sizeof(struct ucell*);
    struct ucell *cell = ucell_alloc(u, ms);

    if( !cell )
        return 0;

    cell->tp = tp;

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


size_t ustring_length(struct ucell *us) {

    return us ? ((struct ustring*)us->data)->len : 0;
}

const char *ustring_cstr(struct ucell *us) {
    return us ? ((struct ustring*)us->data)->data : 0;
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

    return l;
}

static void __dealloc_hstr_item(void *l_, void *k, void *v) {
    struct ulisp *l = l_;
    struct ustring *s = *(struct ustring**)k;
    l->dealloc(l->allocator, s);
}

void ulisp_destroy( struct ulisp *l ) {
    hash_enum(l->hstr, l, __dealloc_hstr_item);
    hash_destroy(l->hstr);
    l->dealloc(l->allocator, l->hstr);
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

    struct exp_tokenizer *tokenizer;

};

size_t ulisp_parser_size() {
    return sizeof(struct ulisp_parser);
}

struct ulisp_parser *ulisp_parser_create( void *mem
                                        , size_t memsize
                                        , read_char_fn rfn
                                        , void *allocator
                                        , alloc_function_t alloc
                                        , dealloc_function_t dealloc
                                        , struct ulisp *u ) {

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
                               };

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

/*static inline struct ucell *mk_str_from_strchunk( struct ulisp *u*/
/*                                                , struct strchunk *cs) {*/
/*    return string( u*/
/*                 , mk_strchunk_reader( pstacktmp(struct strchunk_reader)*/
/*                                     , cs ));*/
/*}*/

/*static inline struct ucell *mk_atom_from_strchunk( struct ulisp *u*/
/*                                                 , struct strchunk *cs) {*/
/*    return atom( u*/
/*                 , mk_strchunk_reader( pstacktmp(struct strchunk_reader)*/
/*                                     , cs ));*/
/*}*/

static void reset_tokenizer( struct ulisp_parser *p, void *reader ) {

    destroy_tokenizer(p);

    p->tokenizer = exp_tokenizer_create( p->allocator
                                       , p->alloc
                                       , p->dealloc
                                       , reader
                                       , p->readfn );

}


static struct ucell *parse_expr( struct ulisp_parser *p, struct ucell *top );

static struct ucell *parse_list( struct ulisp_parser *p ) {
    struct ulisp *u = p->u;
    struct exp_token *tok = token_get(p);

    if( !tok ) {
        // FIXME: parse error, unclosed list?
        assert(0);
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
    struct exp_token *tok = token_get(p);
    struct ulisp *u = p->u;

    switch( tok->tag ) {
        case TOK_INTEGER:
            return integer(u, tok->v.intval);

        case TOK_STRING:
            return cstring_tok(u, tok->v.strval);

        case TOK_ATOM:
            return atom_tok(u, tok->v.atom);

        case TOK_OPAREN:
            return parse_list(p);

        case TOK_CPAREN:
            // TODO: error?
            assert(0);
            return nil;

        case TOK_ERROR:
            // TODO: error?
            assert(0);
            return nil;
    }

    return nil;
}

struct ucell *ulisp_parse( struct ulisp_parser *p, void *what ) {

    fprintf(stderr, "ulisp_parse\n");

    reset_tokenizer(p, what);

    return parse_expr(p, list(p->u, nil));
}


