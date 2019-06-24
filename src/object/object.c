#include <stdio.h>
#include <string.h>

#include "../memory/memory.h"
#include "object.h"
#include "../value/value.h"
#include "../vm/vm.h"

// These kinds of macros exist to avoid having to manullaly cast void* into the actual pointer type you want
#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    return object;
}

static ObjString* allocateString(char* chars, int length) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;

    return string;
}

/**
    Copy chars from source code string into new null terminated string. We do this because not all strings will be
    expressly written literals in the code (e.g. some may be created by string concatenation), so for every string value
    in lox, we allocate a new array of chars on the lox heap (implemented with the C heap)
 */
ObjString* copyString(const char* chars, int length) {
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';

    return allocateString(heapChars, length);
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}