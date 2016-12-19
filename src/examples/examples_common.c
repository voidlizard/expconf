#include "examples_common.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *example_alloc(void *cc, size_t size) {
    return malloc(size);
}

void example_dealloc(void *cc, void *mem) {
    free(mem);
}

bool file_read_char(void *file_, unsigned char *dst) {
    FILE *file = file_;

    if( feof(file) ) {
        return false;
    }

    *dst = (unsigned char)fgetc(file);

    if( feof(file) ) {
        return false;
    }

    return true;
}
