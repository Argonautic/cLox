clox: src/main.o src/chunk/chunk.o src/memory/memory.o src/debug/debug.o  # Dependency files (e.g. main.o, chunk.o) will be created at the path specified here (why?)
	clang -o clox src/main.o src/chunk/chunk.o src/memory/memory.o src/debug/debug.o

main.o: src/main.c src/chunk/chunk.h src/memory/memory.h src/common.h
	clang -c src/main.c

chunk.o: src/chunk/chunk.c src/chunk/chunk.h src/memory/memory.h src/common.h
	clang -c src/chunk/chunk.c

memory.o: src/memory/memory.c src/memory/memory.h src/common.h
	clang -c src/memory/memory.c

debug.o: src/debug/debug.c src/debug/debug.h
	clang -c src/debug/debug.c