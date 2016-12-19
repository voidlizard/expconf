#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expconf.h"
#include "strchunk.h"

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

                char tmp[EXPCONF_LIM_MAX_ATOM] = { 0 };
                const size_t ltmp = sizeof(tmp) - 1;

                fprintf(stderr, "%s(%ld) %s\n"
                              , expconf_token_tag_str(token->tag)
                              , len
                              , strchunk_cstr(token->val.chunk, tmp, ltmp)
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
                              , strchunk_cstr(token->val.chunk, tmp, ltmp)
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
                  ,  expconf_parser_error_num(parser)
           );

    expconf_parser_destroy(&parser);
    expconf_destroy(&exp);

/*    fprintf(stderr, "expconf deallocated\n");*/

    return 0;
}

