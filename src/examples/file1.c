#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expconf.h"
#include "strchunk.h"

#include "dump_token.h"
#include "examples_common.h"

int main(int argc, char *argv[]) {

    const char *fname = "data/file1.conf";

    struct expconf_parser *parser = expconf_parser_create( 0
                                                         , example_alloc
                                                         , example_dealloc );
    assert( parser );

    FILE *file = fopen(fname, "r");

    if( !file ) {
        fprintf(stderr, "*** fatal. %s: %s \n", fname, strerror(errno));
        exit(-1);
    }

    expconf_tokenize( parser
                    , file
                    , file_read_char
                    , 0
                    , debug_dump_token );

__exit:

    expconf_parser_destroy(&parser);
    fclose(file);

    return 1;
}

