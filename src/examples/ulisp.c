#include "ulisp.h"

#include <string.h>
#include <stdarg.h>

#include "hash.h"
#include "hashfun_murmur.h"

struct ulisp {

    struct hash *hstr;

    // allocator boilerplate
    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
};

struct ustring {
    char *s;
    char *e;
    char data[0];
};

struct ucell {
    ucell_type tp;
    struct {
        integer    integer;
        struct ustring  *str;
        struct ucell  *list;
    } car;
    struct ucell *cdr;
};

#define nil ((void*)0)

struct ucell* cons( struct ulisp *l
                  , ucell_type tp
                  , void *car
                  , struct ucell *cdr ) {

    struct ucell *cell = l->alloc(l->allocator, sizeof(struct ucell));

    if( !cell ) {
        return nil;
    }

    cell->tp = tp;
    cell->cdr = cdr;

    switch( tp ) {

        case INTEGER:
            cell->car.integer = (integer)car;
            break;

        case ATOM:
        case STRING:
            cell->car.str = (struct ustring*)car;
            break;

        default:
            cell->car.list = car;
            break;
    };

    return cell;
}

struct ucell* list(struct ulisp *l, ...) {
    va_list ap;
    va_start(ap, l);

    size_t i = 0;

    struct ucell *head = nil;
    struct ucell *curr = nil;

    for(;i < ULISP_LIST_LIT_MAX; i++ ) {
        struct ucell *c = va_arg(ap, struct ucell*);
        if( isnil(c) ) {
            break;
        }

        if( isnil(head) ) {
            curr = head = c;
            continue;
        }

        curr->cdr = c;
        curr = curr->cdr;
    }

    va_end(ap);

    return head;
}

size_t ustring_length(struct ustring *s) {
    return s ? (s->e > s->s ? s->e - s->s - 1 : 0) : 0;
}

const char *ustring_cstr(struct ustring *us) {
    return us ? us->s : 0;
}

static inline struct ustring *mkustring(struct ulisp *l, struct stringreader *cl) {
    const size_t slen = cl->length(cl->cs) + 1;
    struct ustring *us = l->alloc(l->allocator, sizeof(struct ustring) + slen);

    if( !us ) {
        return 0;
    }

    us->s = &us->data[0];
    us->e = &us->data[slen];
    memset(us->data, 0, slen);
    int chr = 0;
    size_t i = 0;
    for( ;cl->readchar(cl->cs, &chr); i++ ) {
        us->s[i] = (char)chr;
    }

    struct ustring **r = hash_get(l->hstr, &us);

    if( r ) {
        l->dealloc(l->allocator, us);
        return *r;
    }

    if( !hash_add(l->hstr, &us, &us) ) {
        l->dealloc(l->allocator, us);
        return 0;
    }

    return us;
}

struct ucell* string(struct ulisp *l, struct stringreader *cl) {
    return cons(l, STRING, mkustring(l, cl), nil);
}

struct ucell* atom(struct ulisp *l, struct stringreader *cl) {
    return cons(l, ATOM, mkustring(l, cl), nil);
}

struct ucell* car( struct ucell *cell ) {
    return 0;
}

struct ucell* cdr( struct ucell *cell ) {
    return 0;
}

size_t ulisp_size() {
    return sizeof(struct ulisp);
}

static uint32_t __pustring_hash(void *a) {
    struct ustring *k = *(struct ustring**)a;
    return hash_murmur3_32(k->s, ustring_length(k), 0x00BEEF);
}

static bool __pustring_eq(void *a, void *b) {
    struct ustring *k1 = *(struct ustring**)a;
    struct ustring *k2 = *(struct ustring**)a;

    size_t k1len = ustring_length(k1);
    size_t k2len = ustring_length(k2);

    return k1len  == k2len && 0 == memcmp(k1->s, k2->s, k1len);
}

void __pustring_cpy(void *a, void *b) {
    *(struct ustring**)a = *(struct ustring**)b;
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
    const size_t MINITEMS = 10;
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

void ucell_walk( struct ulisp *ulisp
               , struct ucell *cell
               , struct ucell_walk_cb *cb ) {


    if( cell == nil ) {
        safeappv(cb->on_list_start, cb->cc);
        cb->on_nil(cb->cc);
        safeappv(cb->on_list_end, cb->cc);
        return;
    }

    safeappv(cb->on_list_start, cb->cc);

    switch( cell->tp ) {
        case INTEGER:
            safeappv(cb->on_integer, cb->cc, cell->car.integer);
            break;

        case LIST:
            ucell_walk(ulisp, cell->car.list, cb);
            break;

        case ATOM:
            safeappv( cb->on_atom
                    , cb->cc
                    , ustring_length(cell->car.str)
                    , ustring_cstr(cell->car.str));
            break;

        case STRING:
            safeappv( cb->on_string
                    , cb->cc
                    , ustring_length(cell->car.str)
                    , ustring_cstr(cell->car.str));
            break;

        default:
            assert(0);
    }

    ucell_walk(ulisp, cell->cdr, cb);
    safeappv(cb->on_list_end, cb->cc);
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


static void reset_tokenizer( struct ulisp_parser *p, void *reader ) {

    destroy_tokenizer(p);

    p->tokenizer = exp_tokenizer_create( p->allocator
                                       , p->alloc
                                       , p->dealloc
                                       , reader
                                       , p->readfn );

}

static struct ucell *parse_expr( struct ulisp_parser *p );

static struct ucell *parse_list( struct ulisp_parser *p ) {
    assert(0);
}

static struct ucell *parse_expr( struct ulisp_parser *p ) {
    assert(0);
}

struct ucell *ulisp_parse( struct ulisp_parser *p, void *what ) {

    fprintf(stderr, "ulisp_parse\n");

    reset_tokenizer(p, what);

    return parse_expr(p);
}
