#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "compiler.h"
#include "../scanner/scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "../debug/debug.h"
#endif

typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

/** Precedence levels sorted lowest to highest */
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . () []
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();  // ParseFn is a func that takes no arguments and returns nothing

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Parser parser;  // Single global variable like vm and scanner. Make factory if using for prod

Chunk* compilingChunk;

static Chunk* currentChunk() {
    return compilingChunk;
}

/**
    Report an error at a particular token. If panic mode activated, do nothing. If panic mode not activated, activate
    panic mode, print an error message, tell parser that the compiler had an error, and skip all errors until we find
    a statement boundary to turn off panic mode

    TODO: Add ability to turn panic mode off once statements are added to cLox
 */
static void errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;  // Stop reporting errors until we reach synchronization point (statement boundary)
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Don't report anything because we're in panic mode
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

/**
    Save current token as previous token, get next token, and save as current token. If the next token is an
    ERROR_TOKEN (represents lexical errors in scanning), report it
 */
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
    Consume a particular TokenType token or report an error if type isn't next to be consumed
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
    emitByte(byte1);
    emitByte(byte2);
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
    #ifdef DEBUG_PRINT_CODE
        if (!parser.hadError) {
            disassembleChunk(currentChunk(), "code");
        }
    #endif
}

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

/**
    Parse a new binary expression. Important to remember that chained binary expressions should be broken down into
    atomic binary expressions. For example, 5 + 5 + 5 + 5 is three separate binary expressions ((5 + 5) + 5) + 5
 */
static void binary() {
    // Remember the operator.
    TokenType operatorType = parser.previous.type;

    // Compile the right operand
    ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));  // parsePrecedence with rule's precedence + 1 because binary expressions are left associative

    // Emit the operator instruction.
    switch (operatorType) {
        case TOKEN_PLUS:    emitByte(OP_ADD); break;
        case TOKEN_MINUS:   emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:    emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH:   emitByte(OP_DIVIDE); break;
        default:
            return; // Unreachable
    }
}

static void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

/**
    Emit one number constant
 */
static void number() {
    double value = strtod(parser.previous.start, NULL);  // Convert previously consumed number token into a double
    emitConstant(value);
}

static void unary() {
    TokenType operatorType = parser.previous.type;

    // Compile the operand
    parsePrecedence(PREC_UNARY);

    // Emit the operator instructions
    switch (operatorType) {
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        default:
            return;  // Unreachable
    }
}

/**
    Rules table for creating expressions. Prefix prop handles which tokens begin a new expression starting from that
    token, infix prop handles which tokens indiciate an expression using previous and succeeding tokens, and precedence
    prop indicates precedence

    rules indices correspond to the enum values of TokenType, which allows us to get a particular token's ParseRule
    using that token's TokenType enum value

    TODO: Not all rules are filled in yet. Until they are, placeholder is { NULL, NULL, PREC_NONE }
 */
ParseRule rules[] = {
    { grouping, NULL,    PREC_CALL },       // TOKEN_LEFT_PAREN
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_PAREN
    { NULL,     NULL,    PREC_NONE },       // TOKEN_LEFT_BRACE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_BRACE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_COMMA
    { NULL,     NULL,    PREC_CALL },       // TOKEN_DOT
    { unary,    binary,  PREC_TERM },       // TOKEN_MINUS
    { NULL,     binary,  PREC_TERM },       // TOKEN_PLUS
    { NULL,     NULL,    PREC_NONE },       // TOKEN_SEMICOLON
    { NULL,     binary,  PREC_FACTOR },     // TOKEN_SLASH
    { NULL,     binary,  PREC_FACTOR },     // TOKEN_STAR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_BANG
    { NULL,     NULL,    PREC_EQUALITY },   // TOKEN_BANG_EQUAL
    { NULL,     NULL,    PREC_NONE },       // TOKEN_EQUAL
    { NULL,     NULL,    PREC_EQUALITY },   // TOKEN_EQUAL_EQUAL
    { NULL,     NULL,    PREC_COMPARISON }, // TOKEN_GREATER
    { NULL,     NULL,    PREC_COMPARISON }, // TOKEN_GREATER_EQUAL
    { NULL,     NULL,    PREC_COMPARISON }, // TOKEN_LESS
    { NULL,     NULL,    PREC_COMPARISON }, // TOKEN_LESS_EQUAL
    { NULL,     NULL,    PREC_NONE },       // TOKEN_IDENTIFIER
    { NULL,     NULL,    PREC_NONE },       // TOKEN_STRING
    { number,   NULL,    PREC_NONE },       // TOKEN_NUMBER
    { NULL,     NULL,    PREC_AND },        // TOKEN_AND
    { NULL,     NULL,    PREC_NONE },       // TOKEN_CLASS
    { NULL,     NULL,    PREC_NONE },       // TOKEN_ELSE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_FALSE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_FOR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_FUN
    { NULL,     NULL,    PREC_NONE },       // TOKEN_IF
    { NULL,     NULL,    PREC_NONE },       // TOKEN_NIL
    { NULL,     NULL,    PREC_OR },         // TOKEN_OR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_PRINT
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RETURN
    { NULL,     NULL,    PREC_NONE },       // TOKEN_SUPER
    { NULL,     NULL,    PREC_NONE },       // TOKEN_THIS
    { NULL,     NULL,    PREC_NONE },       // TOKEN_TRUE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_VAR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_WHILE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_ERROR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_EOF
};

/**
    Parse an expression based on precedence, with *precendence* being the lowest precedence the expression may have
    (higher precedence means higher enum value in the Precedence struct)

    5 + 4 * 2 is parsed in this order:
        - 5 is parsed as a number expression. OP_CONSTANT and index in chunk const array is emitted for const 5
        - + is parsed as an binary expression, because it has higher precedence than the beginning precedence of assignment
        - In the process of parsing +, the second operand is parsed
        - 4 is parsed as a number expression. OP_CONSTANT and index in chunk const array is emitted for const 4
        - * is parsed as a binary expression, because it has a higher precedence than +
        - In the process of parsing *, the second operand is parsed
        - 2 is parsed as a number expression. OP_CONSTANT and index in chunk const array is emitted for const 2
        - Nothing left in full expression to parse, no more tokens consumed
        - At the end of parsing *, OP_MULTIPLY is emitted to multiply the previous two constants (4 and 2)
        - At the end of parsing +, OP_ADD is emitted to add the previous two constants (5 and (4 * 2))
 */
static void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;

    // The first token in an expression will ALWAYS be a prefix expression, whether a literal (like a number) or a unary
    if (prefixRule == NULL) {
        error("Expect expression.");
        return;
    }
    prefixRule();

    // Only continue parsing for infix expressions if the infix expression has greater or equal precedence than
    // *precedence*. If next token has too low precedence, or isn't an infix operator, expression is done and stop
    // advancing
    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

/**
    Get the ParseRule at the index *type*
 */
static ParseRule* getRule(TokenType type) {
    return &rules[type];
}

void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);

    compilingChunk = chunk;
    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}