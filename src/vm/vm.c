#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "../compiler/compiler.h"
#include "../debug/debug.h"
#include "../memory/memory.h"
#include "../object/object.h"
#include "vm.h"

/**
    VM to interpret bytecode created by compiler
 */

// Using a single global VM object for now. If we ever used clox as a production compiler,
// instead pass a pointer to a VM to every function
VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

/**
    Prints an error message to stderr using a format string and any number of corresponding variables. Resets value
    stack afterwards
 */
static void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code;
    // Get line associated with error using the index of the byte
    fprintf(stderr, "[line %d] in script\n", vm.chunk->lines[instruction]);

    resetStack();
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

/**
    Return but don't pop off the value at *distance* distance from the top of the stack
 */
static Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}

/**
    Return the falsiness of a value
 */
static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    ObjString* a = AS_STRING(pop());
    ObjString* b = AS_STRING(pop());

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    push(OBJ_VAL(result));

}

/**
    Run a program. ~90% of clox's time will be spent inside this function

    The expression 5 * 4 + 2 will be sent to VM as the bytecode instructions:

        OP_CONSTANT (5)
        index in chunk's const array (0)
        OP_CONSTANT (4)
        index in chunk's const array (1)
        OP_MULTIPLY
        OP CONSTANT (2)
        OP_ADD
        OP_RETURN

    VM will interpret as:

        - Push new value 5 onto the stack ([ 5 ])
        - Push new value 4 onto the stack ([ 5 ][ 4 ])
        - Pop the previous two values from the stack ()
        - Multiply those values and push the result onto the stack ([ 20 ])
        - Push new value 2 onto the stack ([ 20 ][ 2 ])
        - Pop the previous two values from the stack ()
        - Add those values and push the result onto the stack ([ 22 ])
        - Pop the previous value from th stack ()
        - Return that value (return 22)
 */
static InterpretResult run() {
    #define READ_BYTE() (*vm.ip++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    // valueType refers to a value conversion macro
    #define BINARY_OP(valueType, op) \
        do { \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
                runtimeError("Operands must be numbers."); \
                return INTERPRET_RUNTIME_ERROR; \
            } \
            \
            double b = AS_NUMBER(pop()); \
            double a = AS_NUMBER(pop()); \
            push(valueType(a op b)); \
        } while (false)  // do while loop forces BINARY_OPs to be in their own scope

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
            case OP_NIL: push(NIL_VAL); break;
            case OP_TRUE: push(BOOL_VAL(true)); break;
            case OP_FALSE: push(BOOL_VAL(false)); break;

            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }

            case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;
            case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
            case OP_ADD: {  // Handle both number addition and string concatenation
                if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                    concatenate();
                } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(a + b));
                } else {
                    runtimeError("Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
            case OP_NOT:
                push(BOOL_VAL(isFalsey(pop())));
                break;
            case OP_NEGATE:
                if (!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;  // TODO: Switch to return from function rather than end program execution
            }
        }
    }

    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

/**
    Interpret a single chunk in the vm
 */
InterpretResult interpret(const char* source) {
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}