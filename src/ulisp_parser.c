#include "ulisp_parser.h"
#include "ulisp_parser_internal.h"
#include "miscdata.h"

#define cstring_tok(u, s) \
umake_stringlike((u), STRING, mk_strchunk_reader(pstacktmp(struct strchunk_reader), (s)), true)

#define atom_tok(u, s) \
umake_stringlike((u), ATOM, mk_strchunk_reader(pstacktmp(struct strchunk_reader), (s)), true)

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
                               , .errors = 0
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

static void reset_tokenizer( struct ulisp_parser *p, void *reader ) {

    destroy_tokenizer(p);

    p->tokenizer = exp_tokenizer_create( p->allocator
                                       , p->alloc
                                       , p->dealloc
                                       , p->rdr.cc
                                       , p->readfn );

}

static void parse_error( struct ulisp_parser *p
                       , ulisp_parser_err err
                       , const char *msg) {

    struct ulisp *u = p->u;
    ucell_t *wtf = ulisp_eval_expr(u, atom(u, "__UNIT__"));

    char tmp[ULISP_MAX_ERROR_MSG];
    snprintf(tmp, sizeof(tmp), "%s:%d", ustring_cstr(wtf), (int)p->rdr.lno);

    safecall(unit, p->on_err, p->on_err_cc, err, tmp, msg);
    p->errors++;
}

static struct ucell *parse_expr( struct ulisp_parser *p, struct ucell *top );

static struct ucell *parse_list( struct ulisp_parser *p ) {
    struct ulisp *u = p->u;
    struct exp_token *tok = token_get(p);

    if( !tok ) {
        parse_error(p, ERR__UNBALANCED_PAREN, "");
        return nil;
    }

    if( tok->tag == TOK_CPAREN ) {
        return nil;
    }

    token_unget(p);

    struct ucell *car = parse_expr(p, nil);
    return setlno(p, cons(u, car, parse_list(p)));
}

static struct ucell *parse_expr( struct ulisp_parser *p, struct ucell *top ) {

    do {

        struct exp_token *tok = token_get(p);
        struct ulisp *u = p->u;

        if( !tok ) {
            p->rdr.eof = true;
            return nil;
        }

        switch( tok->tag ) {

            case TOK_NEWLINE:
                p->rdr.lno++;
                continue;

            case TOK_INTEGER:
                return setlno(p, integer(u, tok->v.intval));

            case TOK_STRING:
                return setlno(p, cstring_tok(u, tok->v.strval));

            case TOK_ATOM:
                return setlno(p, atom_tok(u, tok->v.atom));

            case TOK_OPAREN:
                return setlno(p, parse_list(p));

            case TOK_CPAREN:
                parse_error(p, ERR__UNBALANCED_PAREN, "");
                return nil;

            case TOK_SQUOT:
                return setlno(p, quote(u, parse_expr(p, top)));

            case TOK_DOT:
                return setlno(p,atom(u, ","));

            case TOK_COMMA:
                return setlno(p, atom(u, ","));

            case TOK_SEMI:
                return setlno(p, atom(u, ";"));

            case TOK_ERROR:
                parse_error(p, ERR__INVALID_TOKEN, "");
                return nil;

            default:
                assert(0);
        }

    } while(1);

    return nil;
}

static struct ucell *ulisp_parse_(struct ulisp_parser *p) {
    ucell_t *e = parse_expr(p, list(p->u, nil));
    return p->errors ? nil : e;
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

    return p->errors ? nil : head;
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

