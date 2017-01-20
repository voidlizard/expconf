#include "ulisp.h"
#include "ulisp_internal.h"
#include "ulisp_primop_string.h"

struct strlist_reader {
    struct ulisp *u;
    ucell_t *head;
    ucell_t *curr;
    const char *s;
    const char *e;
    size_t len;
    struct stringreader reader;
};

static size_t strlist_length(void *rd_) {
    struct strlist_reader *rd = rd_;
    struct ulisp *u = rd->u;

    if( rd->len )
        return rd->len;

    size_t len = 0;
    ucell_t *o = rd->head;
    ucell_t *s = nil;
    for(; o; o = cdr(o) ) {

        ucell_t *s = car(o);
        ucell_t *sv =  ucell_is_string(s) ? s : ucell_to_string(u, s);
        len += ustring_length(sv);
    }

    rd->len = len;

    return len;
}

static inline bool strlist_read_char(void *rd_, int *chr) {
    struct strlist_reader *rd = rd_;

    struct ulisp *u = rd->u;

    do {

        if( isnil(rd->curr) ) {
            return false;
        }

        if( !rd->s ) {
            ucell_t *se = car(rd->curr);
            ucell_t *ss = ucell_is_string(se) ? se
                                              : ucell_to_string(u, se);
            rd->s = ustring_cstr(ss);
            rd->e = rd->s + ustring_length(ss);
        }


        if( rd->s >= rd->e ) {
            rd->s = 0;
            rd->e = 0;
            rd->curr = cdr(rd->curr);
            continue;
        }

        *chr = *rd->s;
        rd->s++;

        break;

    } while(1);

    return  true;
}

static inline struct stringreader * mk_strlist_reader( struct strlist_reader *rd
                                                     , struct ulisp *u
                                                     , ucell_t *e ) {

    *rd = (struct strlist_reader){ .u = u
                                 , .head = e
                                 , .curr = e
                                 , .s = 0
                                 , .e = 0
                                 , .len = 0
                                 , .reader = { .cs = rd
                                             , .length = strlist_length
                                             , .readchar = strlist_read_char
                                             }
                                 };

    return &rd->reader;
}


ucell_t *ulisp_intersperse(struct ulisp *u, ucell_t *what, ucell_t *list) {
    if( isnil(list) || list->tp != CONS ) {
        return list;
    }

    ucell_t *l  = list;
    ucell_t *h  = nil;
    ucell_t *p  = nil;
    for(; l; l = cdr(l) ) {
        if( isnil(p) ) {
            h = p = cons(u, car(l), nil);
        }
        else {
            ucell_t *n = cons(u, car(l), nil);
            ucell_t *nw = cons(u, what, n);
            setcdr(p, nw);
            p = n;
        }
    }

    return h;
}


ucell_t *ulisp_string_concat(struct ulisp *u, ucell_t *list) {
    ucell_t *empty = cstring_(u, "");

    if( isnil(list) || list->tp != CONS ) {
        return empty;
    }

    return  umake_stringlike( u
                            , STRING
                            , mk_strlist_reader( pstacktmp(struct strlist_reader)
                                               , u
                                               , list), false);

}

static ucell_t* umap( struct ulisp *u
                    , ucell_t *(*fn)(struct ulisp*, ucell_t*)
                    , ucell_t *e) {


    if( isnil(e) || e->tp != CONS ) {
        return nil;
    }

    ucell_t *l  = e;
    ucell_t *h  = nil;
    ucell_t *p  = nil;
    for(; l; l = cdr(l) ) {
        if( isnil(p) ) {
            h = p = cons(u, fn(u,car(l)), nil);
        }
        else {
            ucell_t *n = cons(u, fn(u,car(l)), nil);
            setcdr(p, n);
            p = n;
        }
    }

    return h;
}

ucellp_t ucell_to_string(struct ulisp *u, ucellp_t expr) {

    char tmp256[256];

    if( isnil(expr) ) {
        return cstring(u,"(#nil)");
    }

    switch( expr->tp ) {
        case UNIT:
            return cstring(u,"(#unit)");

        case CONS:
            return ulisp_string_concat(u, list(u, cstring(u, "(")
                                                , ulisp_string_concat(u, ulisp_intersperse( u
                                                                                          , cstring(u, " ")
                                                                                          , umap(u, ucell_to_string, expr)))
                                                , cstring(u, ")")
                                                , nil));

        case INTEGER:
            snprintf(tmp256, sizeof(tmp256), "%ld", ucell_intval(expr));
            return cstring_(u,tmp256);

        case ATOM:
            return expr;

        case STRING:
            return ulisp_string_concat(u, list(u, cstring(u, "\"")
                                                , expr
                                                , cstring(u, "\"")
                                                , nil));

        case TUPLE:
            return cstring(u,"(#tuple)");

        case CLOSURE:
            snprintf(tmp256, sizeof(tmp256), "(#closure %p)", car(expr));
            return cstring_(u,tmp256);

        case PRIMOP:
            snprintf(tmp256, sizeof(tmp256), "(#primop %p)", ucell_primop(expr));
            return cstring_(u, tmp256);

        case OBJECT:
            snprintf(tmp256, sizeof(tmp256), "(#object %p)", ucell_primop(expr));
            return cstring_(u,tmp256);

        default:
            return cstring(u, "(#unknown-object)");
    }

}



