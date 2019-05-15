#include <stdio.h>

#include "debug.h"
#include "../value/value.h"

static int simpleInstruction(const char* name, int offset);

/*
    Disassemble each instruction in a chunk
 */
void disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        // offset is set here instead of in loop instructions because instructions can have varying
        // sizes, and we always want to go to the location of the next instruction
        offset = disassembleInstruction(chunk, offset);
    }
}

int disassembleInstruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);

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
    printValue(chunk->constants.value[constIndex]);
    printf("'\n");
    return offset + 2;
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}