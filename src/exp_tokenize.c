#include "exp_tokenize.h"

#include <assert.h>
#include <string.h>

#include <stdio.h>

struct exp_tokenizer {

    struct {
        bool           eof;
        unsigned char *pc;
        unsigned char  data;
    } read;

    struct {
        struct strchunk chunk;
        unsigned char buf[EXP_LIM_MAX_ATOM+1];
    } atom;

    struct {
        int base;
        int digits;
        integer value;
    } intval;

    struct strchunk *pstr;

    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
    void *reader;
    read_char_fn readchar;
};

static inline unsigned char * __readchar(struct exp_tokenizer *);
static inline void __unread(struct exp_tokenizer*);

struct exp_tokenizer *exp_tokenizer_create( void *allocator
                                          , alloc_function_t  alloc
                                          , dealloc_function_t dealloc
                                          , void *reader
                                          , read_char_fn readchar
                                          ) {

    struct exp_tokenizer *t = alloc(allocator, sizeof(struct exp_tokenizer));

    if( !t ) {
        return 0;
    }

    memset(t, 0, sizeof(struct exp_tokenizer));

    t->allocator = allocator;
    t->alloc = alloc;
    t->dealloc = dealloc;
    t->reader = reader;
    t->readchar = readchar;

    return t;
}


void exp_tokenizer_destroy( struct exp_tokenizer **t ) {
    struct exp_tokenizer *tt = *t;
    strchunk_destroy(tt->pstr, tt->allocator, tt->dealloc);
    tt->dealloc(tt->allocator, *t);
    *t = 0;
}

static void integer_init(struct exp_tokenizer *t, int base) {
    t->intval.base = base;
    t->intval.digits = 0;
    t->intval.value = 0;
}

static void integer_acc(void *t_, unsigned char c) {
    struct exp_tokenizer *t = t_;
    integer inc = 0;

    switch(t->intval.base) {
        case 8:
        case 10:
            inc = c - '0';
            break;

        case 16:
            switch(c) {
                case '0'...'9':
                    inc = c - '0';
                    break;
                case 'a'...'f':
                    inc = 10 + c - 'a';
                    break;
                case 'A'...'F':
                    inc = 10 + c - 'A';
                    break;
            }
            break;
    }

    t->intval.value *= t->intval.base;
    t->intval.value += inc;
    t->intval.digits++;
}

static void atom_init(struct exp_tokenizer *t) {
    strchunk_fixed(&t->atom, sizeof(t->atom));
}

static void atom_append(void *t_, unsigned char c) {
    struct exp_tokenizer *t = t_;
    if( strchunk_used(&t->atom.chunk) < EXP_LIM_MAX_ATOM ) {
        strchunk_append_char(&t->atom.chunk, c, t->allocator, t->alloc);
    }
}

static inline bool eof(struct exp_tokenizer *t) {
    return t->read.eof;
}

static bool is_hspace(void *c, unsigned char chr) {
    return !!strchr(" \t", (int)chr);
}

static bool is_space(void *c, unsigned char chr) {
    return !!strchr(" \t\r\n\v\f", (int)chr);
}

static bool is_space_but_newline(void *c, unsigned char chr) {
    return !!strchr(" \t\r\v\f", (int)chr);
}

static bool is_punct(void *c, unsigned char chr) {
    return !!strchr(".();,", (int)chr);
}

static bool is_line_comment(void *c, unsigned char chr) {
    return chr == '#';
}

static bool is_newline(void *c, unsigned char chr) {
    return chr == '\n';
}

static bool is_token_term(void *c, unsigned char chr) {
    return is_space(0, chr) || is_punct(0, chr);
}

static bool is_atom( void *cc
                   , unsigned char c) {

    return !is_space(0, c) && !is_punct(0, c);
}

static bool is_oct_digit( void *cc
                        , unsigned char c ) {
    switch(c) {
        case '0' ... '7':
            return true;
    }

    return false;
}

static bool is_dec_digit( void *cc
                        , unsigned char c ) {
    switch(c) {
        case '0' ... '9':
            return true;
    }

    return false;
}

static bool is_hex_digit( void *cc
                        , unsigned char c ) {
    switch(c) {
        case '0' ... '9':
        case 'a' ... 'f':
        case 'A' ... 'F':
            return true;
    }

    return false;
}


static void ignore(void *cc, unsigned char c) {
}

static inline void takewhile_n( struct exp_tokenizer *t
                              , int max
                              , void *pred_cc
                              , bool (*pred)(void*,unsigned char)
                              , void *fn_cc
                              , void (*fn)(void*,unsigned char)
                              ) {

    bool all = max < 0;

    for(; (all || max > 0) && !eof(t); max--) {
        unsigned char *pc = __readchar(t);

        if( pc && pred(pred_cc, *pc) ) {
            fn(fn_cc, *pc);
        } else {
            __unread(t);
            break;
        }
    }
}

#define takewhile(t, ccp, pred, ccf, f) takewhile_n((t), (-1), (ccp), (pred), (ccf), (f))

struct inv_pred_cc {
    void *cc;
    bool (*pred)(void *cc, unsigned char c);
};

static bool inv_pred(void *cc_, unsigned char chr) {
    struct inv_pred_cc *cc = cc_;
    return !cc->pred(cc->cc, chr);
}

static inline void takeuntil( struct exp_tokenizer *t
                            , void *pred_cc
                            , bool (*pred)(void*,unsigned char)
                            , void *fn_cc
                            , void (*fn)(void*,unsigned char) ) {

    struct inv_pred_cc icc = { .cc = pred_cc, .pred = pred };
    takewhile(t, &icc, inv_pred, fn_cc, fn);
}

#define skip_space(t) takewhile((t), 0, is_space, 0, ignore)
#define skip_space_but_newline(t) takewhile((t), 0, is_space_but_newline, 0, ignore)
#define skip_hspace(t) takewhile((t), 0, is_hspace, 0, ignore)
#define skip_until(t, pcc, pred) takeuntil((t), (pcc), (pred), 0, ignore)
#define skip_char(t) (void)(__readchar(t))


static void emit_token( struct exp_tokenizer *t
                      , exp_token_tag tag
                      , void *data
                      , struct exp_token *tok ) {


    tok->tag = tag;

    switch(tag) {
        case TOK_ERROR:
        case TOK_OPAREN:
        case TOK_CPAREN:
        case TOK_COMMA:
        case TOK_SEMI:
        case TOK_NEWLINE:
            break;

        case TOK_ATOM:
            tok->v.atom = data;
            break;

        case TOK_INTEGER:
            tok->v.intval = *(integer*)data;
            break;

        case TOK_STRING:
            tok->v.strval = data;
            break;

        default:
            assert(0);
            break;
    }

}


static bool esc( unsigned char chr
               , unsigned char *un
               ) {

    switch( chr ) {
        case '\'':
            *un = '\'';
            return true;
        case '"':
            *un = '\"';
            return true;
        case '?':
            *un = '\?';
            return true;
        case '\\':
            *un = '\\';
            return true;
        case 'a':
            *un = '\a';
            return true;
        case 'b':
            *un = '\b';
            return true;
        case 'f':
            *un = '\f';
            return true;
        case 'n':
            *un = '\n';
            return true;
        case 'r':
            *un = '\r';
            return true;
        case 't':
            *un = '\t';
        case 'v':
            *un = '\v';
            return true;
    }

    return false;
}

static bool unescape( struct exp_tokenizer *t ) {

    unsigned char *pc = __readchar(t);
    unsigned char un = 0;
    unsigned char *pun = 0;

    if( !pc ) {
        return false;
    }

    if( *pc == 'x' || *pc == 'X' ) {
        // hex quot
        integer_init(t, 16);
        takewhile_n(t, 2, 0, is_hex_digit, t, integer_acc);
        if( t->intval.digits ) {
            un = (unsigned char)t->intval.value;
            pun = &un;
        }
    }
    else if( is_oct_digit(0, *pc) ) {
        // octal quot
        integer_init(t, 8);
        integer_acc(t, *pc);
        takewhile_n(t, 2, 0, is_oct_digit, t, integer_acc);
        if( t->intval.digits ) {
            un = (unsigned char)t->intval.value;
            pun = &un;
        }
    }
    else if( esc(*pc, &un) ) {
        pun = &un;
    }
    else {
        return false;
    }

    if( pun ) {
        return strchunk_append_char( t->pstr
                                   , *pun
                                   , t->allocator
                                   , t->alloc );
    }

    return false;

}

static bool scan_string( struct exp_tokenizer *t
                       , unsigned char q
                       , struct exp_token *tok
                       ) {

    if( t->pstr ) {
        strchunk_destroy(t->pstr, t->allocator, t->dealloc);
    }

    t->pstr = strchunk_create(t->allocator, t->alloc);

    if( !t->pstr ) {
        return false;
    }

    for(;!eof(t);) {
        unsigned char *pc = __readchar(t);
        if( !pc ) {
            break;
        }

        if( *pc == '\\' ) {
            // handle quotation
            if( !unescape(t) ) {
                break;
            }
            continue;
        }

        if( *pc == q ) {
            // emit token
            emit_token(t, TOK_STRING, t->pstr, tok);
            return true;
        }

        if( !strchunk_append_char(t->pstr, *pc, t->allocator, t->alloc) ) {
            break;
        }
    }

    emit_token(t, TOK_ERROR, 0, tok);
    return false;
}

struct exp_token * exp_tokenizer_next( struct exp_tokenizer *t
                                     , struct exp_token *tok ) {

    if( eof(t) ) {
        return 0;
    }

    do {
        skip_space_but_newline(t);

        unsigned char *pc = __readchar(t);

        if( !pc )
            return 0;

        if( *pc == '\n' ) {
            emit_token(t, TOK_NEWLINE, 0, tok);
            return tok;
        }

        if( is_line_comment(t, *pc) ) {
            skip_until(t, 0, is_newline);
            skip_char(t);
            continue;
        }

        if( *pc == '(' ) {
            emit_token(t, TOK_OPAREN, 0, tok);
            return tok;
        }

        if( *pc == ')' ) {
            emit_token(t, TOK_CPAREN, 0, tok);
            return tok;
        }

        if( *pc == ',' ) {
            emit_token(t, TOK_COMMA, 0, tok);
            return tok;
        }

        if( *pc == ';' ) {
            emit_token(t, TOK_SEMI, 0, tok);
            return tok;
        }

        if( is_dec_digit(0, *pc) ) {

            integer_init(t, 10);

            if( *pc != '0' ) {
                integer_init(t, 10);
                integer_acc(t, *pc);
                takewhile(t, 0, is_dec_digit, t, integer_acc);
            }

            if( *pc == '0' ) {
                unsigned char *x = __readchar(t);
                if( x && (*x == 'x' || *x == 'X') ) {
                    integer_init(t, 16);
                    takewhile(t, 0, is_hex_digit, t, integer_acc);
                } else if( x && is_oct_digit(0, *x) ) {
                    integer_init(t, 8);
                    integer_acc(t, *x);
                    takewhile(t, 0, is_oct_digit, t, integer_acc);
                }
            }

            __unread(t);
            emit_token(t, TOK_INTEGER, &t->intval.value, tok);
            return tok;
        }

        if( *pc == '"' || *pc == '\'' ) {
            // handle string
            scan_string(t, *pc, tok);
            return tok;
        }

        if( is_atom(0, *pc) ) {
            atom_init(t);
            atom_append(t, *pc);
            takewhile(t, 0, is_atom, t, atom_append);
            emit_token(t, TOK_ATOM, &t->atom.chunk, tok);
            return tok;
        }


        break;

    } while(1);

    emit_token(t, TOK_ERROR, 0, tok);
    return tok;
}

static inline void __unread(struct exp_tokenizer* t) {
    t->read.pc = &t->read.data;
}

static inline unsigned char * __readchar(struct exp_tokenizer * t) {

    if( t->read.eof ) {
        return 0;
    }

    // unread?
    if( t->read.pc == &t->read.data ) {
        t->read.pc = 0;
    } else {
        if( !t->readchar(t->reader, &t->read.data) ) {
            t->read.eof = true;
            return 0;
        }
    }

    return &t->read.data;
}

const char *exp_token_tag_name(exp_token_tag tag) {
    switch(tag) {
        case TOK_ERROR:
            return "ERROR";
        case TOK_COMMA:
            return "TOK_COMMA";
        case TOK_SEMI:
            return "TOK_SEMI";
        case TOK_NEWLINE:
            return "TOK_NEWLINE";
        case TOK_OPAREN:
            return "OPAREN";
        case TOK_CPAREN:
            return "CPAREN";
        case TOK_ATOM:
            return "ATOM";
        case TOK_INTEGER:
            return "INTEGER";
        case TOK_STRING:
            return "STRING";
        default:
            assert(0);
    }
}

