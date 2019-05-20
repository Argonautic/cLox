#include <stdio.h>

#include "../common.h"
#include "../debug/debug.h"
#include "vm.h"

// Using a single global VM object for now. If we ever used clox as a production compiler,
// instead pass a pointer to a VM to every function
VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
}

void freeVM() {

}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

/*
    Run a program. ~90% of clox's time will be spent inside this function
 */
static InterpretResult run() {
    #define READ_BYTE() (*vm.ip++)  // I'm guessing these are defined as macros for call efficiency
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    for (;;) {
        #ifdef  DEBUG_TRACE_EXECUTION
            printf("          ");
            for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }
            printf("\n");
            disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));  // Get address OFFSET between start of code chunk and current instruction
        #endif

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;  // TODO: Switch to return from function rather than end program execution
            }
        }
    }

    #undef READ_BYTE
    #undef READ_CONSTANT
}

/*
    Interpret a single chunk in the vm
 */
InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}