#ifndef clox_vm_h
#define clox_vm_h

#include "../chunk/chunk.h"

typedef struct {
    Chunk* chunk;
    uint8_t* ip;  // Instruction Pointer. Pointer to the location of the NEXT instruction in the bytecode array. Faster to deref a pointer than get array index
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(Chunk* chunk);

#endif
