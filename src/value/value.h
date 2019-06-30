/*
    Module for representing constant values. Instead of storing values in the bytecode stream,
    constant values will be stored in a separate "constant pool" region of the executable, and
    code stream will have references to the location of specific values
 */

#ifndef clox_value_h
#define clox_value_h

#include "../common.h"

// Forward declarations of Obj structs to get around circular dependencies
typedef struct sObj Obj;
typedef struct sObjString ObjString;

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ  // Any Lox value that lives in the heap at runtime
} ValueType;

/**
    Struct to represent a Lox value's type. *type* holds the enum value of the type and the union holds the actual value

    TODO: Improve memory efficiency of type tag
*/
typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        Obj* obj;
    } as;
} Value;

// Macros to check the ValueType of a Value
#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

// Macros to get C primitives out of Value *as* fields. Should always be guarded with the corresponding IS_TYPE macro
#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)
#define AS_OBJ(value)     ((value).as.obj)

// Macros to instantiate new Values from C primitives
#define BOOL_VAL(value)   ((Value){ VAL_BOOL, { .boolean = value } })
#define NIL_VAL           ((Value){ VAL_NIL, { .number = 0 } })
#define NUMBER_VAL(value) ((Value){ VAL_NUMBER, { .number = value } })
#define OBJ_VAL(value)    ((Value){ VAL_OBJ, { .obj = value } })

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;  // This is the constant pool

bool valuesEqual(Value a, Value b);
void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif