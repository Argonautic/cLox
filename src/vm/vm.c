#include <stdio.h>

#include "../common.h"
#include "vm.h"

static InterpretResult run();

// Using a single global VM object for now. If we ever used clox as a production compiler,
// instead pass a pointer to a VM to every function
VM vm;

void initVM() {

}

void freeVM() {

}

/*
    Interpret a single chunk in the vm
 */
InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

/*
    Run a program. ~90% of clox's time will be spent inside this function
 */
static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)  // I'm guessing these are defined as macros for call efficiency
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    for (;;) {
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                printValue(constant);
                printf("\n");
                break;
            }
            case OP_RETURN: {
                return INTERPRET_OK;  // TODO: Switch to return from function rather than end program execution
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}