#ifndef __ulisp_primop_string_h
#define __ulisp_primop_string_h

ucellp_t ucell_to_string(struct ulisp *u, ucellp_t expr);
ucell_t *ulisp_string_concat(struct ulisp *u, ucell_t *list);

#endif
