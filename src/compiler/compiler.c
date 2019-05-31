#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "compiler.h"
#include "../scanner/scanner.h"

typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

Parser parser;  // Single global variable like vm and scanner. Make factory if using for prod

Chunk* compilingChunk;

static Chunk* currentChunk() {
    return compilingChunk;
}

static void errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;  // Stop reporting errors until we reach synchronization point (statement boundary)
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {

    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);  // print token itself
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void error(const char*message) {
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message) {
    errorAt(&parser.current, message);
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        // Scan until you find a non error token
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}

/**
    Consume a particular TokenType or report error if type isn't next to be consumed
 */
static void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

/**
    Write one byte to the chunk
 */
static void emitByte(uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);  // previous line used for error reporting
}

/**
    Emit two bytes consecutively. Convenience function for emitting opcode byte + operand byte
 */
static void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(1);
    emitByte(2);
}

/**
    Emit function return
 */
static void emitReturn() {
    emitByte(OP_RETURN);
}

/**
    Add a constant to the chunk constant array and check if too many constants
 */
static uint8_t makeConstant(Value value) {
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void endCompiler() {
    emitReturn();
}

/**
    Emit one number constant
 */
static void number() {
    double value = strtod(parser.previous.start, NULL);  // Convert previously consumed number token into a double
    emitConstant(value);
}

void expression() {

}

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}