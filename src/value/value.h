/*
    Module for representing constant values. Instead of storing values in the bytecode stream,
    constant values will be stored in a separate "constant pool" region of the executable, and
    code stream will have references to the location of specific values
 */

#ifndef clox_value_h
#define clox_value_h

#include "../common.h"

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER
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
    } as;
} Value;

// Macros to check the ValueType of a Value
#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value),type == VAL_NIL)
#define IS_NUMBER(value)         ((value).type == VAL_NUMBER)

// Macros to get C primitives out of Value *as* fields. Should always be guarded with the corresponding IS_TYPE macro
#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)

// Macros to instantiate new Values from C primitives
#define BOOL_VAL(value)   ((Value){ VAL_BOOL, { .boolean = value } })
#define NIL_VAL           ((Value){ VAL_NIL, { .number = 0 } })
#define NUMBER_VAL(value) ((Value){ VAL_NUMBER, { .number = value } })

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;  // This is the constant pool

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif