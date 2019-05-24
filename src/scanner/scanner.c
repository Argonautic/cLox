#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

/**
    Unlike jLox, which eagerly scans all Tokens and hands off to the Parser in one go, cLox scans tokens
    as needed and passes them by value to the compiler. This is to be more memory efficient and to avoid
    the memory management of using a potentially huge list of tokens one by one
 */

Scanner scanner;  // Like the vm, we use a single global scanner object, and like the VM, this should change if used for production code

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = scanner.current - scanner.start;  // The book casts this value as an int but I can't tell why that's necessary
    token.line = scanner.line;

    return token;
}

/**
    Create an error token. Points to a message instead of a place in the source code, but the message will
    always be a string literal that has static storage duration so we don't need to manually free it
 */
static Token errorToken(const char *message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);  // This makes sense to cast because strlen returns a size_t
    token.line = scanner.line;

    return token;
}

/**
    Scan one new token
 */
Token scanToken() {
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    // TODO: Make clox process valid characters. Right now REPL triggers an infinite loop of "Unexpected character."

    return errorToken("Unexpected character.");
}