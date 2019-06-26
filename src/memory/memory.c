#include <stdlib.h>

#include "../common.h"
#include "memory.h"
#include "../vm/vm.h"

/**
    Handle all cases of memory management in clox. Allocates new blocks, frees up existing blocks, and resizes existing
    blocks
 */
void* reallocate(void* previous, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(previous);
        return NULL;
    }

    return realloc(previous, newSize);  // equivalent to malloc if oldSize == 0
}

static void freeObject(Obj* object) {
    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(ObjString, object);
            break;
        }
    }
}

/**
    Free all heap allocated objects created by the compiler & vm
 */
void freeObjects() {
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
}