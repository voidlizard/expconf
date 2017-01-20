#ifndef __ulisp_types_h
#define __ulisp_types_h

#include "ulisp_platform.h"

typedef enum {
     UNIT
   , CONS
   , INTEGER
   , ATOM
   , STRING
   , TUPLE
   , CLOSURE
   , PRIMOP
   , ANY
   , OBJECT
} ucell_type;

struct ulisp;
struct ucell;
struct utuple;

typedef struct ucell ucell_t;
typedef ucell_t* ucellp_t;
typedef struct utuple utuple_t;
typedef void* object;
typedef void* eval_context;

#endif
