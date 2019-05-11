#ifndef clox_memory_h
#define clox_memory_h

// These are macros because they need to support different types and C is butt at that
// Eventually replace with inline functions, but it might be tough since you can't
// pass in a type as a variable
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(previous, type, oldCount, count) \
    (type*)reallocate(previous, sizeof(type) * (oldCount), sizeof(type) * count)

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* previous, size_t, size_t newSize);

#endif