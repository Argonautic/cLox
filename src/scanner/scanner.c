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
    Unlike jLox, which eagerly scans all Tokens and hands off to the Parser in one go, cLox scans tokens as needed and
    passes them by value to the compiler. This is to be more memory efficient and to avoid the memory management of
    using a potentially huge list of tokens one by one

    Also unique to cLox - the scanner simply delineates which chars in the source code string tokens begin and end at.
    It creates no objects to hold additional values and little analysis can be done here. Each call to advance()
    advances the current char pointer, each call to makeToken() adds a new Token based on the difference between the
    current char pointer and start char pointer, and each call to scanToken() sets start char pointer equal to current
 */

Scanner scanner;  // Like the vm, we use a single global scanner object, and like the VM, this should change if used for production code

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAlpha(char c) {
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static char peek() {
    return *scanner.current;
}

/**
    Peek one character ahead of current (two total characters from last scanned character)
 */
static char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;

    scanner.current++;
    return true;
}

/**
    Return a new token. Token is represented by type, start index in source string, length in source string, and line #
    in source code. Each makeToken() call resets where the "current" char pointer points to
 */
static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = scanner.current - scanner.start;  // The book casts this value as an int but I can't tell why that's necessary
    token.line = scanner.line;

    return token;
}

/**
    Create an error token. Points to a message instead of a place in the source code, but the message will always be a
    string literal that has static storage duration so we don't need to manually free it
 */
static Token errorToken(const char *message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);  // This makes sense to cast because strlen returns a size_t
    token.line = scanner.line;

    return token;
}

static void skipWhiteSpace() {
    for (;;) {
        char c = peek();
        switch(c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;

            case '\n':
                scanner.line++;
                advance();
                break;

            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;

            default:
                return;
        }
    }
}

/**
    Check if part of an identifier token just scanned matches a keyword. A little awkward to understand because we only
    compare branches of keywords (e.g. we check for string "lass" instead of "class") but it's slightly more efficient.
    (Why do we use memcmp instead of strcmp? If the two strings are different lengths, they're guaranteed invalid)
 */
static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {

        return type;
    }
}

/**
    Check for whether the current identifier token is actually a reserved keyword
 */
static TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

/**
    Scan the entire identifier token and then return a TokenType depending on whether the token is a keyword or not
 */
static Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    return makeToken(identifierType());
}

/**
    Process a number literal. Conversion from token lexeme to runtime number value happens in compilation
 */
static Token number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

/**
    Process a string literal. Conversion from token lexeme to runtime string value happens in compilation
 */
static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    // The closing "
    advance();
    return makeToken(TOKEN_STRING);
}

/**
    Scan one new token
 */
Token scanToken() {
    skipWhiteSpace();

    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RiGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '!':
            return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

        case '"': return string();
    }

    return errorToken("Unexpected character.");
}