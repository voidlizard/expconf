#ifndef __ulisp_parser_internal_h
#define __ulisp_parser_internal_h

#include "ulisp_internal.h"

struct ulisp_parser {
    read_char_fn readfn;

    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
    struct ulisp *u;

    struct {
        struct exp_token   mem;
        struct exp_token  *curr;
        struct exp_token  *back;
    } token;

    struct {
        size_t lno;
        void *cc;
        bool eof;
    } rdr;

    struct exp_tokenizer *tokenizer;
    void *on_err_cc;
    ulisp_parser_err_fn on_err;
    int errors;
};

static inline ucell_t *setlno(struct ulisp_parser *p, ucell_t *cell) {
	cell->lno = p->rdr.lno;
	return cell;
}

#endif
