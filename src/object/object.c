#include <stdio.h>
#include <string.h>

#include "../memory/memory.h"
#include "object.h"
#include "../value/value.h"
#include "../vm/vm.h"

/**
    Shortcut macro to allocate and get a new Object. *type* refers to the type of Object pointer you get back, and
    objectType refers to the value of the ObjType type field that exists on the base Obj struct. Macro exists to avoid
    having to manullaly cast void* into desired pointer type every time
 */
#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

/**
    Allocate space for a new lox Object and set its type equal to *type*
 */
static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;

    object->next = vm.objects;  // Add to the heap objects linked list
    vm.objects = object;
    return object;
}

/**
    Allocate space for a new lox String Object and set its fields appropriately
 */
static ObjString* allocateString(char* chars, int length, uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;

    return string;
}

/**
    Implementation of an FNV-1a hash on a string
 */
static uint32_t hashString(const char* key, int length) {
    uint32_t hash = 2166136261u;

    for (int i = 0; i < length; i++) {
        hash ^= key[i];
        hash *= 16777619;
    }

    return hash;
}

/**
    Create a new object using an existing string in memory. Used to transfer ownership of strings between StrObjs
 */
ObjString* takeString(char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    return allocateString(chars, length, hash);
}

/**
    Copy chars from source code string into new null terminated string. We do this because not all strings will be
    expressly written literals in the code (e.g. some may be created by string concatenation), so for every string value
    in lox, we allocate a new array of chars on the lox heap (implemented with the C heap)
 */
ObjString* copyString(const char* chars, int length) {
    uint32_t hash = hashString(chars, length);

    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';

    return allocateString(heapChars, length, hash);
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}