#ifndef clox_compiler_h
#define clox_compiler_h

#include "../object/object.h"
#include "../vm/vm.h"

bool compile(const char* source, Chunk* chunk);

#endif
