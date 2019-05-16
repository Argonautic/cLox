#include <stdio.h>

#include "debug.h"
#include "../value/value.h"

static int constantInstruction(const char* name, Chunk* chunk, int offset);
static int simpleInstruction(const char* name, int offset);

/*
    Disassemble each instruction in a chunk and print debug info. Print info will be in the format:

    == test chunk ==
    0000  123 OP_CONSTANT         0 '1.2'
    0002    | OP_RETURN

    Which corresponds to a 3 byte chunk. First byte is the instructions to create a constant, second byte
    is the index of that constant in the chunk's constant array, and third byte is the instruction for a
    return. All bytes correspond to code from line 123
 */
void disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        // offset is set here instead of in loop instructions because instructions can have varying
        // sizes, and we always want to go to the location of the next instruction
        offset = disassembleInstruction(chunk, offset);
    }
}

/*
    Disassemble one instruction
 */
int disassembleInstruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {  // Same line as previous byte
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
    uint8_t constIndex = chunk->code[offset + 1];  // refers to index of where constant is stored in ValueArray
    printf("%-16s %4d '", name, constIndex);
    printValue(chunk->constants.values[constIndex]);
    printf("'\n");
    return offset + 2;
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}