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