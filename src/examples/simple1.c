#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <stdbool.h>
#include <stddef.h>

// FIXME: move to top-level header

typedef void* (*alloc_function_t)(void*, size_t);
typedef void  (*dealloc_function_t)(void*, void*);

typedef size_t (*read_block_t)(void*, size_t);
typedef bool   (*read_char_t)(void*, unsigned char *);

// FIXME: move to header

#define EXPCONF_LIM_MAX_ATOM 512

typedef size_t expconf_integer;

typedef enum {
     ERR_NO_ERROR
   , ERR_TOK_UNKNOWN
   , ERR_TOK_BAD_ATOM
   , ERR_TOK_BAD_NUMBER
   , ERR_TOK_STRING_TOO_LARGE
} expconf_parser_error;

struct expconf;
struct expconf_parser;

struct expconf *expconf_create( void *allocator
                              , alloc_function_t alloc
                              , dealloc_function_t dealloc );

void expconf_destroy( struct expconf **conf );

struct expconf_parser * expconf_parser_create( void *allocator
                                             , alloc_function_t alloc
                                             , dealloc_function_t dealloc );


void expconf_parser_destroy( struct expconf_parser ** );

void expconf_set_parser_error( struct expconf_parser *p, expconf_parser_error err);

// FIXME: move to impl.

#include <string.h>

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

typedef enum {
     EXPCONF_TOKEN_ATOM
   , EXPCONF_TOKEN_STRING
   , EXPCONF_TOKEN_INTEGER
} expconf_token_tag;

struct expconf_token {
    expconf_token_tag tag;
    union {
        expconf_integer ei;
        struct strchunk *chunk;
    } val;
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

typedef void (*on_token_t)(void *cc, struct expconf_token *);

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

static inline bool expconf_parser_ok(struct expconf_parser *p) {
    return (p->errors == 0);
}

static inline void expconf_parser_line_inc(struct expconf_parser *p) {
    p->lineno++;
}


// to miscdata ?

struct strchunk {
    struct strchunk *next;
    unsigned char *sp;
    unsigned char *se;
    unsigned char sdata[0];
};

inline size_t strchunk_used( struct strchunk *s ) {
    return s->sp - s->sdata;
}

struct strchunk * strchunk_create( void *allocator
                                 , alloc_function_t alloc
                                 ) {

    const size_t chunksize = 4096;
    const size_t ssize = sizeof(struct strchunk);

    struct strchunk *s = alloc(allocator, chunksize);

    if( !s ) {
        return 0;
    }

    memset(s, 0, chunksize);

    s->sp = &s->sdata[0];

    const size_t smax = chunksize > ssize ? chunksize - ssize : 0;

    s->se = &s->sdata[smax];

    return s;
}

void strchunk_destroy( struct strchunk *chunk
                     , void *allocator
                     , dealloc_function_t dealloc  ) {

    struct strchunk *head = chunk;

    while( head ) {
        struct strchunk *tmp = head;
        head = head->next;
        dealloc(allocator, tmp);
    }
}

bool strchunk_avail( struct strchunk *chunk ) {
    return chunk->sp < chunk->se;
}


size_t strchunk_length( struct strchunk *chunk_ ) {
    struct strchunk *chunk = chunk_;
    size_t len = 0;

    for(; chunk; chunk = chunk->next ) {
        len += strchunk_used(chunk);
    }

    return len;
}

void strchunk_cstr(struct strchunk *chunk, unsigned char *dst, size_t len) {
    unsigned char *dp = dst;
    unsigned char *de = &dst[len-1];

    for(; dp < de && chunk; chunk = chunk->next ) {
        unsigned char *p = chunk->sdata;
        unsigned char *pe = chunk->sp;
        for(; p < pe && dp < de; p++, dp++ ) {
            *dp = *p;
        }
    }

    *dp = 0;
}

bool strchunk_append_char( struct strchunk *chunk_
                         , unsigned char c
                         , void *allocator
                         , alloc_function_t alloc
                         ) {

    struct strchunk *s = strchunk_avail(chunk_) ? chunk_
                                                : strchunk_create(allocator, alloc);

    if( !s ) {
        return false;
    }

    *s->sp = c;
     s->sp++;

    return true;
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
  , DECNUM_START
  , ATOM_START
  , SQ_STRING_START
  , DQ_STRING_START
  , COMMENT_START
} tok_state_t;

struct tok_state {
    tok_state_t state;
    struct strchunk *tok;
    expconf_integer  decnum;
};

static inline void __cleanup_state( struct expconf_parser *p
                                  , struct tok_state *state ) {

    strchunk_destroy(state->tok, p->allocator, p->dealloc);
}

static inline void __to_state( struct expconf_parser *p
                             , tok_state_t nstate
                             , struct tok_state *state
                             , unsigned char input
                             ) {
    state->state = nstate;
    switch( state->state ) {

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
    struct tok_state state = { .state = INITIAL
                             , .tok = 0
                             , .decnum = 0
                             };

    unsigned char chr = 0;
    for(; readfn(reader, &chr) && expconf_parser_ok(p); ) {

        if( __newline(p, chr) ) {
            expconf_parser_line_inc(p);
        }

        switch( state.state ) {
            case INITIAL:

                if( __line_comment_start(p, chr) ) {
                    __to_state(p, COMMENT_START, &state, chr);
                    break;
                }

                if( __atom_start_chr(p, chr) ) {
                    __to_state(p, ATOM_START, &state, chr);
                    break;
                }

                if( chr == '\'' ) {
                    __to_state(p, SQ_STRING_START, &state, chr);
                    break;
                }

                if( chr == '"' ) {
                    __to_state(p, DQ_STRING_START, &state, chr);
                    break;
                }

                if( __dec_digit(p, chr, 0) ) {
                    __to_state(p, DECNUM_START, &state, chr);
                    break;
                }

                if( __space(p, chr) || __newline(p, chr) ) {
                    break;
                }

                expconf_set_parser_error(p, ERR_TOK_UNKNOWN);

                break;

            case DECNUM_START:

                {
                    int v = 0;
                    if( __dec_digit(p, chr, &v) ) {
                        state.decnum *= 10;
                        state.decnum += (expconf_integer)v;
                        break;
                    }

                    if( __newline(p, chr) || __space(p, chr) ) {
                        __flush_token(p, EXPCONF_TOKEN_INTEGER, (void*)state.decnum, tok_cc, on_token);
                        __to_state(p, INITIAL, &state, chr);
                        break;
                    }
                }

                expconf_set_parser_error(p, ERR_TOK_BAD_NUMBER);

                break;

            case SQ_STRING_START:

                if( chr == '\'' ) {
                    __flush_token(p, EXPCONF_TOKEN_STRING, state.tok, tok_cc, on_token);
                    __to_state(p, INITIAL, &state, chr);
                    break;
                }

                if( !__acc_string(p, state.tok, chr) ) {
                    expconf_set_parser_error(p, ERR_TOK_STRING_TOO_LARGE);
                }

                break;

            case DQ_STRING_START:

                if( chr == '"' ) {
                    // flush token
                    __flush_token(p, EXPCONF_TOKEN_STRING, state.tok, tok_cc, on_token);
                    __to_state(p, INITIAL, &state, chr);
                    break;
                }

                if( !__acc_string(p, state.tok, chr) ) {
                    expconf_set_parser_error(p, ERR_TOK_STRING_TOO_LARGE);
                }

                break;

            case ATOM_START:
                if( __newline(p, chr) ) {
                    __flush_token(p, EXPCONF_TOKEN_ATOM, state.tok, tok_cc, on_token);
                    __to_state(p, INITIAL, &state, chr);
                    break;
                }

                if( __space(p, chr) ) {
                    __flush_token(p, EXPCONF_TOKEN_ATOM, state.tok, tok_cc, on_token);
                    __to_state(p, INITIAL, &state, chr);
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
                    __to_state(p, INITIAL, &state, chr);
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

// this file

void *__dumb_alloc(void *cc, size_t size) {
    return malloc(size);
}

void __dumb_dealloc(void *cc, void *mem) {
    free(mem);
}

struct char_reader_ss {
    bool   eof;
    bool   endline;
    size_t line;
    unsigned char *p;
    unsigned char **src;
};


struct char_reader_ss *char_reader_ss_create(struct char_reader_ss *r, unsigned char **ss) {
    memset(r, 0, sizeof(struct char_reader_ss));
    r->eof = false;
    r->endline = false;
    r->src = ss;
    r->line = 0;
    r->p = r->src[r->line];
    return r;
}

bool char_reader_ss_read(void *cc, unsigned char *dst) {
    struct char_reader_ss *reader = cc;

    if( reader->eof ) {
        return false;
    }

    if( !reader->endline ) {
        *dst = *(reader->p);
    } else {
        *dst = '\n';
        reader->endline = false;
        return true;
    }

    reader->p++;

    if( ! (*reader->p) ) {
        reader->endline = true;
        reader->line++;
        reader->p = reader->src[reader->line];
        if( !reader->p ) {
            reader->eof = true;
        }
    }

    return true;
}

unsigned char *test1[] = { "# expconf file example"
                         , "# comment line"
                         , "do-nothing 'string-literal' 123456"
                         , "echo atom ### commented part of string"
                         , "t0123"
                         , "1234 jopa"
                         , "'random string'"
                         , 0
                         };

static void __dump_token(void *cc, struct expconf_token *token) {
    switch( token->tag ) {
        case EXPCONF_TOKEN_INTEGER:
            {
                fprintf(stderr, "%s %ld\n"
                              , expconf_token_tag_str(token->tag)
                              , token->val.ei
                              );
            }
            break;
        case EXPCONF_TOKEN_ATOM:
            {
                struct strchunk *tok = token->val.chunk;
                const size_t len = strchunk_length(tok);
                fprintf(stderr, "%s(%ld) %s\n"
                              , expconf_token_tag_str(token->tag)
                              , len
                              , tok->sdata
                              );
            }
            break;
        case EXPCONF_TOKEN_STRING:
            {
                struct strchunk *tok = token->val.chunk;
                const size_t len = strchunk_length(tok);

                char tmp[128] = { 0 };
                const size_t ltmp = sizeof(tmp) - 1;

                strchunk_cstr(token->val.chunk, tmp, ltmp);
                fprintf(stderr, "%s(%ld) '%s%s'\n"
                              , expconf_token_tag_str(token->tag)
                              , len
                              , tmp
                              , len < ltmp ? "" : ".."
                              );
            }
            break;
    }
}

int main(int argc, char **argv) {

    struct expconf *exp = expconf_create( 0
                                        , __dumb_alloc
                                        , __dumb_dealloc );

    assert( exp );

/*    fprintf(stderr, "expconf allocated\n");*/


    struct expconf_parser *parser = expconf_parser_create( 0
                                                         , __dumb_alloc
                                                         , __dumb_dealloc );

    assert( parser );

/* TODO: implement this    */
/*    expconf_bind( exp*/
/*                , atom("do-nothing")*/
/*                , instance(c_string)*/
/*                , instance(c_int)*/
/*                , 0 // context*/
/*                , do_nothing*/
/*                );*/


    struct char_reader_ss ssreader = { 0 };
    expconf_tokenize( parser
                    , char_reader_ss_create(&ssreader, test1)
                    , char_reader_ss_read
                    , 0
                    , __dump_token );

    fprintf(stdout, "parse errors: %ld\n"
                  , parser->errors
           );

    expconf_parser_destroy(&parser);
    expconf_destroy(&exp);

/*    fprintf(stderr, "expconf deallocated\n");*/

    return 0;
}

