#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;  // Like the vm, we use a single global scanner object, and like the VM, this should change if used for production code

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}