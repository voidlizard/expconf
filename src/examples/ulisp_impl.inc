#ifndef __ulisp_impl_inc
#define __ulisp_impl_inc

struct ulisp {

    // FIXME: remove obsolete
    struct hash *hstr;

    struct hash *dict;

	// error handling
	void *error_cc;
	ulisp_on_eval_error error_fn;

    // allocator boilerplate
    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;

	int errors;
    jmp_buf *abrt;
};

struct ucell {
	int32_t lno;
    ucell_type tp;
    struct ucell *data[1];
};

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

#define setcar(cell, v) (cell)->data[0] = (v)
#define setcdr(cell, v) (cell)->data[1] = (v)
#define car(cell) (cell)->data[0]
#define cdr(cell) (cell)->data[1]

static inline ucell_t *setlno(struct ulisp_parser *p, ucell_t *cell) {
	cell->lno = p->rdr.lno;
	return cell;
}

#endif
