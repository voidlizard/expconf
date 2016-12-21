#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exp_tokenize.h"
#include "strchunk.h"
#include "examples_common.h"

int main(int argc, char *argv[]) {


    if( argc < 2 ) {
        fprintf(stderr, "usage: tokenize file-name\n");
        exit(1);
    }

    const char *fname = argv[1];

    FILE *file = fopen(fname, "rb");

    if( !file ) {
        fprintf(stderr, "*** fatal. %s: %s \n", fname, strerror(errno));
        exit(-1);
    }

    struct exp_tokenizer *tokenizer =
            exp_tokenizer_create( 0
                                , example_alloc
                                , example_dealloc
                                , file
                                , file_read_char
                                );


    assert( tokenizer );


    for(;;) {
        struct exp_token token = { 0 };
        if( !exp_tokenizer_next(tokenizer, &token) ) {
            break;
        }

        switch( token.tag ) {
            case TOK_STRING:
                {
                    const size_t len = strchunk_length(token.v.strval);

                    char tmp[128] = { 0 };
                    const size_t ltmp = sizeof(tmp) - 1;

                    fprintf(stdout, "%s(%ld) '%s%s'\n"
                                  , exp_token_tag_name(token.tag)
                                  , len
                                  , strchunk_cstr(token.v.strval, tmp, ltmp)
                                  , len < ltmp ? "" : ".."
                                  );
                }
                break;


            case TOK_ATOM:
                {
                    char tmp[EXP_LIM_MAX_ATOM+1];
                    fprintf(stdout, "%s(%ld) %s\n"
                                  , exp_token_tag_name(token.tag)
                                  , strchunk_used(token.v.atom)
                                  , strchunk_cstr(token.v.atom, tmp, sizeof(tmp))
                           );
                }
                break;

            case TOK_INTEGER:
                {
                    fprintf(stdout, "%s %ld\n"
                                  , exp_token_tag_name(token.tag)
                                  , token.v.intval
                           );
                }
                break;


            default:
                fprintf(stdout, "%s\n", exp_token_tag_name(token.tag));
                break;
        }

    }

/*    expconf_tokenize( parser*/
/*                    , file*/
/*                    , file_read_char*/
/*                    , 0*/
/*                    , debug_dump_token );*/

/*    fprintf(stdout, "parse errors: %ld\n"*/
/*                  ,  expconf_parser_error_num(parser)*/
/*           );*/

__exit:

    exp_tokenizer_destroy(&tokenizer);
    fclose(file);

    return 0;
}

