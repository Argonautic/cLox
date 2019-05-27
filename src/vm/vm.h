#ifndef clox_vm_h
#define clox_vm_h

#include "../chunk/chunk.h"
#include "../value/value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;  // Instruction Pointer. Pointer to the location of the NEXT instruction in the bytecode array. Faster to deref a pointer than get array index
    Value stack[STACK_MAX];
    Value* stackTop;  // Like ip, points at value after last pushed stack value (or to 0 if nothing is on stack)
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

#endif
