#ifndef __example_alloc_h
#define __example_alloc_h

#include <stddef.h>
#include <stdio.h>

void *example_alloc(void *cc, size_t size);
void example_dealloc(void *cc, void *mem);

bool file_read_char(void *file_, unsigned char *dst);

#endif
