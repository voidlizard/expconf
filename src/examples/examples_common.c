#include "examples_common.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *example_alloc(void *cc, size_t size) {
    return malloc(size);
}

void example_dealloc(void *cc, void *mem) {
    free(mem);
}

bool file_read_char(void *file_, unsigned char *dst) {
    FILE *file = file_;

    if( feof(file) ) {
        return false;
    }

    *dst = (unsigned char)fgetc(file);

    if( feof(file) ) {
        return false;
    }

    return true;
}

void dump_token(FILE *to, struct exp_token *token) {

    switch( token->tag ) {
        case TOK_STRING:
            {
                const size_t len = strchunk_length(token->v.strval);

                char tmp[128] = { 0 };
                const size_t ltmp = sizeof(tmp) - 1;

                fprintf(to, "%s(%ld) '%s%s'\n"
                              , exp_token_tag_name(token->tag)
                              , len
                              , strchunk_cstr(token->v.strval, tmp, ltmp)
                              , len < ltmp ? "" : ".."
                              );
            }
            break;


        case TOK_ATOM:
            {
                char tmp[EXP_LIM_MAX_ATOM+1];
                fprintf(to, "%s(%ld) %s\n"
                              , exp_token_tag_name(token->tag)
                              , strchunk_used(token->v.atom)
                              , strchunk_cstr(token->v.atom, tmp, sizeof(tmp))
                       );
            }
            break;

        case TOK_INTEGER:
            {
                fprintf(to, "%s %ld\n"
                              , exp_token_tag_name(token->tag)
                              , token->v.intval
                       );
            }
            break;


        default:
            fprintf(to, "%s\n", exp_token_tag_name(token->tag));
            break;
    }

}

