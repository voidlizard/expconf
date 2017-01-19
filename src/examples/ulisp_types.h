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
   , OBJECT
} ucell_type;

struct ulisp;
struct ucell;
struct utuple;

typedef struct ucell ucell_t;
typedef struct utuple utuple_t;

#endif
