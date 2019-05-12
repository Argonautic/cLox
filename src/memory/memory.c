#include <stdlib.h>

#include "../common.h"
#include "memory.h"

/*
    Handle all cases of memory management in clox. Allocates new blocks,
    frees up existing blocks, and resizes existing blocks
 */
void* reallocate(void* previous, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(previous);
        return NULL;
    }

    return realloc(previous, newSize);  // equivalent to malloc if oldSize == 0
}