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
};

struct ucell {
    ucell_type tp;
    struct ucell *data[1];
};

#define setcar(cell, v) (cell)->data[0] = (v)
#define setcdr(cell, v) (cell)->data[1] = (v)
#define car(cell) (cell)->data[0]
#define cdr(cell) (cell)->data[1]

#endif
