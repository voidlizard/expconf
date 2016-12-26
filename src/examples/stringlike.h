#ifndef __stringlike_h
#define __stringlike_h

#include <string.h>

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
    rd->reader.cs = rd;
    rd->reader.length = __cstring_reader_length;
    rd->reader.readchar = __cstring_reader_readchar;
    return &rd->reader;
}

#endif
