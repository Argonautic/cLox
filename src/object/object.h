#ifndef clox_object_h
#define clox_object_h

#include "../common.h"
#include "../value/value.h"

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_STRING(value)    isObjType(value, OBJ_STRING)

#define AS_STRING(value)    ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)   (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
    OBJ_STRING
} ObjType;

struct sObj {
    ObjType type;
    struct sObj* next;  // Pointer to the next object in the heap. Temp fix to keep track of memory before implementing GC
};

// Important that the Obj is the first field, because C will store obj first in memory, which means you can cast
// *ObjString into *Obj and access Obj fields. Obj will be used compositionally as the first field in all object types
struct sObjString {
    Obj obj;
    int length;
    char* chars;
};

ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void printObject(Value value);

/**
    Checks for if a Value represents a certain ObjType. Allows you to safely cast *Obj to *ObjType
 */
static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif