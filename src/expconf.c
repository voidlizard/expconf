#include "expconf.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "strchunk.h"

struct expconf {
    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
};


struct expconf *expconf_create( void *allocator
                              , alloc_function_t alloc
                              , dealloc_function_t dealloc ) {

    struct expconf *conf = alloc(allocator, sizeof(struct expconf));

    if( !conf ) {
        return 0;
    }

    memset(conf, 0, sizeof(struct expconf));
    conf->allocator = allocator;
    conf->alloc = alloc;
    conf->dealloc = dealloc;

    return conf;
}

void expconf_destroy( struct expconf **conf ) {
    struct expconf *e = *conf;
    e->dealloc(e->allocator, *conf);
    *conf = 0;
}

struct expconf_parser {
    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;

    size_t errors;
    size_t lineno;
};


const char *expconf_token_tag_str(expconf_token_tag tag) {
    switch(tag) {
        case EXPCONF_TOKEN_ATOM:
            return "ATOM";
        case EXPCONF_TOKEN_STRING:
            return "STRING";
        case EXPCONF_TOKEN_INTEGER:
            return "INTEGER";
    }
    assert(0);
}


struct expconf_parser * expconf_parser_create( void *allocator
                                             , alloc_function_t alloc
                                             , dealloc_function_t dealloc ) {

    struct expconf_parser *p = alloc(allocator, sizeof(struct expconf_parser));

    if( !p ) {
        return 0;
    }

    memset(p, 0, sizeof(struct expconf_parser));
    p->allocator = allocator;
    p->alloc = alloc;
    p->dealloc = dealloc;

    p->errors = 0;
    p->lineno = 1;

    return p;
}

void expconf_parser_destroy( struct expconf_parser **p ) {
    struct expconf_parser *parser = *p;
    parser->dealloc(parser->allocator, *p);
    *p = 0;
}

size_t expconf_parser_error_num(struct expconf_parser *p) {
    return p->errors;
}

static inline bool expconf_parser_ok(struct expconf_parser *p) {
    return (p->errors == 0);
}

static inline void expconf_parser_line_inc(struct expconf_parser *p) {
    p->lineno++;
}


static inline bool __atom_start_chr( struct expconf_parser *p
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

static inline bool __atom_chr( struct expconf_parser *p
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

static inline bool __oct_digit( struct expconf_parser *p
                              , unsigned char c
                              , int *value ) {

    switch(c) {
        case '0' ... '7':
            if( value ) {
                *value = (int)(c - '0');
            }
            return true;
    }
    return false;

}

static inline bool __dec_digit( struct expconf_parser *p
                              , unsigned char c
                              , int *value ) {
    switch(c) {
        case '0' ... '9':
            if( value ) {
                *value = (int)(c - '0');
            }
            return true;
    }

    return false;
}


static inline bool __hex_digit( struct expconf_parser *p
                              , unsigned char c
                              , int *value ) {
    switch(c) {
        case '0' ... '9':
            if( value ) {
                *value = (int)(c - '0');
            }
            return true;

        case 'a' ... 'f':
            if( value ) {
                *value = (int)(10 + c - 'a');
            }
            return true;

        case 'A' ... 'F':
            if( value ) {
                *value = (int)(10 + c - 'A');
            }
            return true;

    }

    return false;
}

static bool __esc( struct expconf_parser *p
                 , unsigned char chr
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

static inline bool __line_comment_start( struct expconf_parser *p
                                       , unsigned char chr) {
    return chr == '#';
}

static inline bool __newline( struct expconf_parser *p
                            , unsigned char chr) {
    return chr == '\n';
}

static inline bool __space( struct expconf_parser *p
                          , unsigned char chr) {

    switch(chr) {
        case ' ':
            return true;
        case '\t':
            return true;
        defaule:
            return false;
    }
    return false;
}

static inline bool __token_break( struct expconf_parser *p
                                , unsigned char chr ) {

    if( __space(p, chr) )
        return true;

    if( __newline(p, chr) )
        return true;

/*    if( __obrace(p, chr) ) */
/*        return true;*/

/*    if( __cbrace(p, chr) ) */
/*        return true;*/

    return false;
}

static inline void __inc_errors( struct expconf_parser *p ) {
    p->errors++;
}

void expconf_set_parser_error( struct expconf_parser *p, expconf_parser_error err) {
    __inc_errors(p);
}

static inline void __flush_token( struct expconf_parser *p
                                , expconf_token_tag tag
                                , void *tok_
                                , void *tok_cc
                                , on_token_t on_token ) {


    switch( tag ) {
        case EXPCONF_TOKEN_INTEGER:
            {
                struct expconf_token token = { .tag = tag
                                             , .val.ei = (expconf_integer)tok_
                                             };
                on_token(tok_cc, &token);
            }
            break;

        case EXPCONF_TOKEN_STRING:
        case EXPCONF_TOKEN_ATOM:
            {
                struct expconf_token token = { .tag = tag
                                             , .val.chunk = tok_
                                             };
                on_token(tok_cc, &token);
            }
            break;
    }

}

static inline bool __acc_string( struct expconf_parser *p
                               , struct strchunk *tok
                               , unsigned char c ) {

    return strchunk_append_char(tok, c, p->allocator, p->alloc);
}

static inline bool __acc_token( struct expconf_parser *p
                              , struct strchunk *tok
                              , unsigned char c ) {

    size_t ltok = strchunk_length(tok);

    if( ltok >= EXPCONF_LIM_MAX_ATOM ) {
        return false;
    }

    bool ret = strchunk_append_char(tok, c, p->allocator, p->alloc);
    return ret;
}

typedef enum {
    INITIAL
  , NUM_START
  , DECNUM_START
  , HEXNUM_START
  , ATOM_START
  , SQ_STRING_START
  , DQ_STRING_START
  , CHR_ESC
  , CHR_ESC_HEX
  , CHR_ESC_OCT
  , COMMENT_START
} tok_state_t;

struct tok_state {
    tok_state_t prev;
    tok_state_t state;
    struct strchunk *tok;
    expconf_integer  decnum;

    struct {
        int base;
        int i;
        int val;
    } quot;

};

static inline void __cleanup_state( struct expconf_parser *p
                                  , struct tok_state *state ) {

    strchunk_destroy(state->tok, p->allocator, p->dealloc);
}


static inline void __jump_state( struct expconf_parser *p
                             , tok_state_t nstate
                             , struct tok_state *state
                             , unsigned char input
                             ) {

    state->prev = state->state;
    state->state = nstate;
}

static inline void __call_state( struct expconf_parser *p
                             , tok_state_t nstate
                             , struct tok_state *state
                             , unsigned char input
                             ) {

    state->prev = state->state;
    state->state = nstate;

    switch( state->state ) {

        case NUM_START:
            state->decnum =  0;
            break;


        case HEXNUM_START:
            state->decnum = 0;
            break;

        case DECNUM_START:
            {
                int v = 0;
                __dec_digit(p, input, &v);
                state->decnum = v;
            }
            break;

        case ATOM_START:
        case DQ_STRING_START:
        case SQ_STRING_START:
            strchunk_destroy(state->tok, p->allocator, p->dealloc);
            state->tok = strchunk_create(p->allocator, p->alloc);
            break;

        case CHR_ESC:
            memset(&state->quot, 0, sizeof(state->quot));
            break;

    }


    if( state->state == ATOM_START ) {
        strchunk_append_char(state->tok, input, p->allocator, p->alloc);
    }
}

void expconf_tokenize( struct expconf_parser *p
                     , void *reader
                     , read_char_t readfn
                     , void *tok_cc
                     , on_token_t on_token
                     ) {


    bool stop = false;
    bool unread = false;

    struct tok_state state = { .prev  = INITIAL
                             , .state = INITIAL
                             , .tok = 0
                             , .decnum = 0
                             };

    unsigned char chr = 0;
    for( ;!stop; ) {

        if( !expconf_parser_ok(p) ) {
            break;
        }

        if( unread ) {
            unread = false;
        } else if( !readfn(reader, &chr)) {
            break;
        }

        if( __newline(p, chr) ) {
            expconf_parser_line_inc(p);
        }

        switch( state.state ) {
            case INITIAL:

                if( __line_comment_start(p, chr) ) {
                    __call_state(p, COMMENT_START, &state, chr);
                    break;
                }

                if( __atom_start_chr(p, chr) ) {
                    __call_state(p, ATOM_START, &state, chr);
                    break;
                }

                if( chr == '\'' ) {
                    __call_state(p, SQ_STRING_START, &state, chr);
                    break;
                }

                if( chr == '"' ) {
                    __call_state(p, DQ_STRING_START, &state, chr);
                    break;
                }

                if( chr == '0' ) {
                    __call_state(p, NUM_START, &state, chr);
                    break;
                }

                if( __dec_digit(p, chr, 0) && chr != '0' ) {
                    __call_state(p, DECNUM_START, &state, chr);
                    break;
                }

                if( __space(p, chr) || __newline(p, chr) ) {
                    break;
                }

                fprintf(stderr, "WUT? %c %02x\n", chr, chr);

                expconf_set_parser_error(p, ERR_TOK_UNKNOWN);

                break;

            case NUM_START:

                if( chr == 'x' || chr == 'X' ) {
                    __call_state(p, HEXNUM_START, &state, chr);
                    break;
                }

                expconf_set_parser_error(p, ERR_TOK_BAD_NUMBER);
                break;

            case DECNUM_START:

                {
                    int v = 0;
                    if( __dec_digit(p, chr, &v) ) {
                        state.decnum *= 10;
                        state.decnum += (expconf_integer)v;
                        break;
                    }

                    if( __token_break(p, chr) ) {
                        __flush_token(p, EXPCONF_TOKEN_INTEGER, (void*)state.decnum, tok_cc, on_token);
                        __call_state(p, INITIAL, &state, chr);
                        break;
                    }
                }

                expconf_set_parser_error(p, ERR_TOK_BAD_NUMBER);

                break;

            case HEXNUM_START:

                {
                    int v = 0;
                    if( __hex_digit(p, chr, &v) ) {
                        state.decnum *= 16;
                        state.decnum += (expconf_integer)v;
                        break;
                    }

                    if( __token_break(p, chr) ) {
                        __flush_token(p, EXPCONF_TOKEN_INTEGER, (void*)state.decnum, tok_cc, on_token);
                        __call_state(p, INITIAL, &state, chr);
                        break;
                    }
                }

                expconf_set_parser_error(p, ERR_TOK_BAD_NUMBER);

                break;


            case SQ_STRING_START:

                if( chr == '\\' ) {
                    __call_state(p, CHR_ESC, &state, chr);
                    break;
                }

                if( chr == '\'' ) {
                    __flush_token(p, EXPCONF_TOKEN_STRING, state.tok, tok_cc, on_token);
                    __call_state(p, INITIAL, &state, chr);
                    break;
                }

                if( !__acc_string(p, state.tok, chr) ) {
                    expconf_set_parser_error(p, ERR_TOK_STRING_TOO_LARGE);
                }

                break;

            case DQ_STRING_START:


                if( chr == '\\' ) {
                    __call_state(p, CHR_ESC, &state, chr);
                    break;
                }

                if( chr == '"' ) {
                    // flush token
                    __flush_token(p, EXPCONF_TOKEN_STRING, state.tok, tok_cc, on_token);
                    __call_state(p, INITIAL, &state, chr);
                    break;
                }

                if( !__acc_string(p, state.tok, chr) ) {
                    expconf_set_parser_error(p, ERR_TOK_STRING_TOO_LARGE);
                }

                break;

            case CHR_ESC:
                {
                    unsigned char unesc = 0;

                    if( __esc(p, chr, &unesc) ) {
                        if( !__acc_string(p, state.tok, unesc) ) {
                            expconf_set_parser_error(p, ERR_TOK_STRING_TOO_LARGE);
                            break;
                        }
                        __jump_state(p, state.prev, &state, chr);
                        break;
                    }

                    if( chr == 'X' || chr == 'x' ) {
                        tok_state_t old = state.prev;
                        __jump_state(p, CHR_ESC_HEX, &state, chr);
                        state.prev = old;
                        state.quot.base = 16;
                        break;
                    }

                    int val = 0;

                    if( __oct_digit(p, chr, &val) ) {
                        tok_state_t old = state.prev;
                        __jump_state(p, CHR_ESC_OCT, &state, chr);
                        state.prev = old;
                        state.quot.base = 8;
                        state.quot.val = val;
                        state.quot.i++;
                        break;
                    }

                }

                expconf_set_parser_error(p, ERR_TOK_BAD_STRING);
                break;

            case CHR_ESC_OCT:
                {
                    int val = 0;
                    bool ok = false;
                    if( __oct_digit(p, chr, &val) ) {
                        state.quot.val *= 8;
                        state.quot.val += val;
                        state.quot.i++;
                        ok = true;
                    }

                    if( state.quot.i >= 3 || !ok ) {
                        if( !__acc_string(p, state.tok, (unsigned char)state.quot.val) ) {
                            expconf_set_parser_error(p, ERR_TOK_STRING_TOO_LARGE);
                            break;
                        }
                        __jump_state(p, state.prev, &state, chr);

                        if( !ok ) {
                            unread = true;
                        }

                        break;
                    }
                }
                break;

            case CHR_ESC_HEX:
                {
                    int val = 0;
                    bool ok = false;
                    if( __hex_digit(p, chr, &val) ) {
                        state.quot.val *= 16;
                        state.quot.val += val;
                        state.quot.i++;
                        ok = true;
                    }

                    if( state.quot.i >= 2 || !ok ) {
                        if( !__acc_string(p, state.tok, (unsigned char)state.quot.val) ) {
                            expconf_set_parser_error(p, ERR_TOK_STRING_TOO_LARGE);
                            break;
                        }
                        __jump_state(p, state.prev, &state, chr);

                        if( !ok ) {
                            unread = true;
                        }

                        break;
                    }
                }
                break;

            case ATOM_START:
                if( __token_break(p, chr) ) {
                    __flush_token(p, EXPCONF_TOKEN_ATOM, state.tok, tok_cc, on_token);
                    __call_state(p, INITIAL, &state, chr);
                    break;
                }

                if( __atom_chr(p, chr) ) {
                    // acc token
                    if( !__acc_token(p, state.tok, chr) ) {
                        expconf_set_parser_error(p, ERR_TOK_BAD_ATOM);
                    }
                    break;
                }

                expconf_set_parser_error(p, ERR_TOK_BAD_ATOM);
                break;

            case COMMENT_START:
                if( __newline(p, chr) ) {
                    __call_state(p, INITIAL, &state, chr);
                    break;
                }
                break;

            default:
                assert( 0 );
                break;
        }
    }

    __cleanup_state(p, &state);

}

