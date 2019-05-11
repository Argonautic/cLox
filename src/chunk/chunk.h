/* Module to define code representation - chunk refers to a blob of bytecode */

#ifndef clox_chunk_h
#define clox_chunk_h

#include "../common.h"

// OpCode refers to OperationCode, the type of an instruction being run
typedef enum {
    OP_RETURN,
} OpCode;


// Chunk will hold a series of instructions, along with other related data
// Stored as dynamic array
typedef struct {
    int count;  // Count and capacity for dynamic array purposes
    int capacity;
    uint8_t* code;  // Using uint8_t to represent bytes
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte);

#endif