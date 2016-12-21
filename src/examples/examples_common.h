#ifndef __example_alloc_h
#define __example_alloc_h

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "exp_tokenize.h"

void *example_alloc(void *cc, size_t size);
void example_dealloc(void *cc, void *mem);

bool file_read_char(void *file_, unsigned char *dst);

void dump_token(FILE *to, struct exp_token *tok);

#endif
