#ifndef clox_memory_h
#define clox_memory_h

// Eventually replace GROW_CAPACITY and GROW_ARRAY with inline functions
// Might be tough since you can't pass in a type as a variable
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(previous, type, oldCount, count) \
    (type*)reallocate(previous, sizeof(type) * (oldCount), sizeof(type) * count)

void* reallocate(void* previous, size_t, size_t newSize);

#endif