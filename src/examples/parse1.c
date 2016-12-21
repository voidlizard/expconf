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
        fprintf(stderr, "usage: parse file-name\n");
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

        dump_token(&token);

    }

__exit:

    exp_tokenizer_destroy(&tokenizer);
    fclose(file);

    return 0;
}

