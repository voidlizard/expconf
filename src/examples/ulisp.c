#include "ulisp.h"

#include <string.h>

struct ulisp {

    // allocator boilerplate
    void *allocator;
    alloc_function_t alloc;
    dealloc_function_t dealloc;
};

struct ucell {
    ucell_type tp;
    struct {
        integer    integer;
        struct ucell *list;
    } car;
    struct ucell *cdr;
};

#define nil ((void*)0)

struct ucell* cons( struct ulisp *l
                  , ucell_type tp
                  , void *car
                  , struct ucell *cdr ) {

    struct ucell *cell = l->alloc(l->allocator, sizeof(struct ucell));

    if( !cell ) {
        return nil;
    }

    cell->tp = tp;
    cell->cdr = cdr;

    switch( tp ) {

        case INTEGER:
            cell->car.integer = (integer)car;
            break;

        default:
            cell->car.list = car;
            break;
    };

    return cell;
}

struct ucell* car( struct ucell *cell ) {
    return 0;
}

struct ucell* cdr( struct ucell *cell ) {
    return 0;
}

size_t ulisp_size() {
    return sizeof(struct ulisp);
}

struct ulisp *ulisp_create( void *mem
                          , size_t memsize
                          , void *allocator
                          , alloc_function_t alloc
                          , dealloc_function_t dealloc ) {


    if( memsize < ulisp_size() ) {
        return 0;
    }

    struct ulisp *l = mem;
    memset(l, 0, ulisp_size());

    SET_ALLOCATOR(l, allocator, alloc, dealloc);

    return l;
}

void ulisp_destroy( struct ulisp *ulisp ) {
}


void ucell_walk( struct ulisp *ulisp
               , struct ucell *cell
               , struct ucell_walk_cb *cb ) {


    if( cell == nil ) {
        safeappv(cb->on_list_start, cb->cc);
        cb->on_nil(cb->cc);
        safeappv(cb->on_list_end, cb->cc);
        return;
    }

    safeappv(cb->on_list_start, cb->cc);

    switch( cell->tp ) {
        case INTEGER:
            safeappv(cb->on_integer, cb->cc, cell->car.integer);
            break;

        case LIST:
            ucell_walk(ulisp, cell->car.list, cb);
            break;

        default:
            assert(0);
    }

    ucell_walk(ulisp, cell->cdr, cb);
    safeappv(cb->on_list_end, cb->cc);

}

