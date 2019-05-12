#include "common.h"
#include "./chunk/chunk.h"
#include "./debug/debug.h"

int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);
    writeChunk(&chunk, OP_RETURN);

    disassembleChunk(&chunk, "test_chunk");
    freeChunk(&chunk);

    return 0;
}