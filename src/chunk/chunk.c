#include <stdlib.h>

#include "chunk.h"
#include "../memory/memory.h"
#include "../value/value.h"

/*
    Initialize the values of a chunk. Array allocated on first writeChunk
 */
void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    initValueArray(&(chunk->constants));
}

/*
    Deallocate a chunk
 */
void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    freeValueArray(&(chunk->constants));
    initChunk(chunk);
}

/*
    Add a new byte of instruction to the chunk
 */
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        // Double the capacity of chunk array
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(chunk->code, uint8_t, oldCapacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(chunk->lines, int, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

/*
    Add a constant to the constants array and return the index it was added into

    TODO: implement OP_CONSTANT_LONG to store constants with indices greater than 255
          (maybe store three bytes sequentially in *code* with values between 0 and 255)
 */
int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&(chunk->constants), value);
    return chunk->constants.count - 1;
}