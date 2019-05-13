#ifndef clox_value_h
#define clox_Value_h

#include "../common.h"

/*
    Instead of storing values in the bytecode stream, constant values will be stored
    in a separate "constant pool" region of the executable, and
 */

typedef double Value;  // begin by supporting double precision floats until we add support for other values

typedef struct valueArray ValueArray;  // This is the constant pool

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array);
void freeValueArray(ValueArray* array);

#endif