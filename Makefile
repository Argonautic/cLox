CC ?= clang

objects = src/main.o src/chunk/chunk.o src/memory/memory.o src/value/value.o src/debug/debug.o \
          src/vm/vm.o src/compiler/compiler.o src/scanner/scanner.o

clox: $(objects)
	cc -o clox $(objects)

src/main.o: src/chunk/chunk.h src/memory/memory.h src/debug/debug.h src/common.h
src/chunk/chunk.o: src/memory/memory.h src/common.h
src/memory/memory.o: src/common.h
src/value/value.o: src/memory/memory.h src/common.h
src/debug/debug.o: src/chunk/chunk.h src/value/value.h
src/vm/vm.o: src/chunk/chunk.h src/common.h src/value/value.h src/debug/debug.h src/compiler/compiler.h
src/compiler/compiler.o: src/scanner/scanner.h src/vm/vm.h src/debug/debug.h
src/scanner/scanner.o: