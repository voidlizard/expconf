#include "dump_token.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

void debug_dump_token(void *cc, struct expconf_token *token) {
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

