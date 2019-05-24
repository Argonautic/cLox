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

Token scanToken() {

}