#include <stdio.h>

#include "../common.h"
#include "compiler.h"
#include "../scanner/scanner.h"

void compile(const char* source) {
    initScanner(source);

    // TODO: Replace temp code with actual compilation
    int line = -1;
    for (;;) {
        Token token = scanToken();
        if (token.line != line) {
            printf("%d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);  // .*s prints the first token.length chars of token.start

        if (token.type == TOKEN_EOF) break;
    }
}