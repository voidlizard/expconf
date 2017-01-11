#ifndef __stringlike_h
#define __stringlike_h

#include <string.h>
#include "strchunk.h"

struct stringreader {
    void *cs;
    size_t (*length)(void *cs);
    bool   (*readchar)(void *cs, int *c);
};


struct cstring_reader {
    char *p;
    char *pe;
    struct stringreader reader;
};

static inline size_t __cstring_reader_length(void *rd_) {
    struct cstring_reader *rd = rd_;
    return rd->pe - rd->p;
}

static inline bool __cstring_reader_readchar(void *rd_, int *chr) {
    struct cstring_reader *rd = rd_;
    if( rd->p < rd->pe ) {
        *chr = *rd->p;
        rd->p++;
        return true;
    }

    return false;
}

static inline struct stringreader * mk_cstring_reader( struct cstring_reader *rd
                                                     , char *s ) {

    rd->p = s;
    rd->pe = &rd->p[strlen(s)];
    rd->reader = (struct stringreader){ .cs = rd
                                      , .length = __cstring_reader_length
                                      , .readchar = __cstring_reader_readchar
                                      };

    return &rd->reader;
}

struct strchunk_reader {
    struct strchunk *chunk;
    unsigned char   *cp;
    struct stringreader reader;
};

static size_t __strchunk_length(void *rd_) {
    struct strchunk_reader *rd = rd_;
    return strchunk_length(rd->chunk);
}

static inline bool __strchunk_read_char(void *rd_, int *chr) {
    struct strchunk_reader *rd = rd_;

    do {

        if( !rd->chunk ) break;

        if( rd->cp >= rd->chunk->p ) {
            rd->chunk = rd->chunk->next;
            if( !rd->chunk ) break;
            rd->cp = rd->chunk->data;
        }

        if( rd->cp && rd->cp < rd->chunk->p ) {
            *chr = *rd->cp;
            rd->cp++;
        }

        return true;

    } while(0);

    return false;
}

static inline struct stringreader * mk_strchunk_reader( struct strchunk_reader *rd
                                                      , struct strchunk *c ) {
    rd->chunk = c;
    rd->cp = c->data;

    rd->reader = (struct stringreader){ .cs = rd
                                      , .length = __strchunk_length
                                      , .readchar = __strchunk_read_char
                                      };

    return &rd->reader;
}

#endif
