#include "common.h"
#include "./chunk/chunk.h"
#include "./debug/debug.h"
#include "./vm/vm.h"

int main(int argc, const char* argv[]) {
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    // tests with arbitrary values and line numbers
    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_RETURN, 123);

    disassembleChunk(&chunk, "test_chunk");
    interpret(&chunk);
    freeVM();
    freeChunk(&chunk);

    return 0;
}