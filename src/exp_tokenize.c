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
    tt->dealloc(tt->allocator, *t);
    *t = 0;
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

static bool is_line_comment(void *c, unsigned char chr) {
    return chr == '#';
}

static bool is_newline(void *c, unsigned char chr) {
    return chr == '\n';
}

static bool is_atom_start( void *cc
                         , unsigned char c) {
    switch(c) {
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '_':
        case '$':
        case '@':
            return true;
        default:
            return false;
    }
    return false;
}

static inline bool is_atom( void *cc
                          , unsigned char c) {
    switch(c) {
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '0' ... '9':
        case '.':
        case '-':
        case '_':
        case '$':
        case '@':
            return true;
        default:
            return false;
    }
    return false;
}


static void ignore(void *cc, unsigned char c) {
}

static inline void takewhile( struct exp_tokenizer *t
                            , void *pred_cc
                            , bool (*pred)(void*,unsigned char)
                            , void *fn_cc
                            , void (*fn)(void*,unsigned char)
                            ) {

    for(; !eof(t); ) {
        unsigned char *pc = __readchar(t);

        if( pc && pred(pred_cc, *pc) ) {
            fn(fn_cc, *pc);
        } else {
            __unread(t);
            break;
        }
    }
}

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
            break;

        case TOK_ATOM:
            tok->v.atom = data;
            break;
    }

}

bool exp_tokenizer_next( struct exp_tokenizer *t
                       , struct exp_token *tok ) {

    do {
        skip_space(t);

        unsigned char *pc = __readchar(t);

        if( !pc )
            return false;

        if( is_line_comment(t, *pc) ) {
            skip_until(t, 0, is_newline);
            skip_char(t);
            continue;
        }

        if( *pc == '(' ) {
            emit_token(t, TOK_OPAREN, 0, tok);
            return true;
        }

        if( *pc == ')' ) {
            emit_token(t, TOK_CPAREN, 0, tok);
            return true;
        }

        if( is_atom_start(0, *pc) ) {
            atom_init(t);
            atom_append(t, *pc);
            takewhile(t, 0, is_atom, t, atom_append);
            emit_token(t, TOK_ATOM, &t->atom.chunk, tok);
            return true;
        }

        break;

    } while(1);

    return false;
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
        case TOK_OPAREN:
            return "OPAREN";
        case TOK_CPAREN:
            return "CPAREN";
        case TOK_ATOM:
            return "ATOM";
        default:
            assert(0);
    }
}

