# Dependency files (e.g. main.o, chunk.o) will be created at the path specified in rule dependency (why?)
clox: src/main.o src/chunk/chunk.o src/memory/memory.o src/value/value.o src/debug/debug.o src/vm/vm.o src/compiler/compiler.o src/scanner/scanner.o
	clang -o clox src/main.o src/chunk/chunk.o src/memory/memory.o src/value/value.o src/debug/debug.o src/vm/vm.o src/compiler/compiler.o src/scanner/scanner.o

main.o: src/main.c src/chunk/chunk.h src/memory/memory.h src/common.h
	clang -c src/main.c

chunk.o: src/chunk/chunk.c src/chunk/chunk.h src/memory/memory.h src/common.h
	clang -c src/chunk/chunk.c

memory.o: src/memory/memory.c src/memory/memory.h src/common.h
	clang -c src/memory/memory.c

value.o: src/value/value.c src/value/value.h src/memory/memory.h src/common.h
	clang -c src/value/value.c

debug.o: src/debug/debug.c src/debug/debug.h src/chunk/chunk.h src/value/value.h
	clang -c src/debug/debug.c

vm.o: src/vm/vm.c src/vm/vm.h src/chunk/chunk.h src/common.h src/value/value.h src/debug/debug.h src/compiler/compiler.h
	clang -c src/vm/vm.c

compiler.o: src/compiler/compiler.c src/compiler/compiler.h src/scanner/scanner.h
	clang -c src/compiler/compiler.c

scanner.o: src/scanner/scanner.c src/scanner/scanner.h
	clang -c src/scanner/scanner.c