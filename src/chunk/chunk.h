/**
    Module to define code representation - chunk refers to a blob of bytecode
*/

#ifndef clox_chunk_h
#define clox_chunk_h

#include "../common.h"
#include "../value/value.h"

// Literals whose values can't be enumerated (like strings & numbers) are denoted by OP_CONSTANT codes. The next byte
// after an OP_CONSTANT OpCode is an int that refers to where the constant is stored in the ValueArray

/**
    OpCode refers to OperationCode, the type of an instruction being run. First byte of any instruction is always an
    opcode
*/
typedef enum {
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,  // No separate tokens for !=, <=, and >= since those can be represented with compound bytecode instructions (e.g. !(x < y)), but would be good for performance!
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_RETURN
} OpCode;


/** Chunk will hold a series of instructions, along with other related data. Stored as dynamic array */
typedef struct {
    int count;  // Count and capacity for dynamic array purposes
    int capacity;
    uint8_t* code;  // Using uint8_t to represent bytes
    int* lines;  // Array of code line location for each byte - index of a line corresponds to the index of a byte in *code*
    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);

#endif