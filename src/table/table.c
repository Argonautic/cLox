#include <stdlib.h>
#include <string.h>

#include "../memory/memory.h"
#include "../object/object.h"
#include "table.h"
#include "../value/value.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void freeTable(Table* table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

/**
    Find an entry in your hash table by key. Hash table uses linear probing to find an entry in destination bucket or
    forward adjacent buckets. Uses a pointer to Entries array rather than pointer to Table because some operations have
    to be done on Entries arrays that aren't part of Tables yet
 */
static Entry* findEntry(Entry* entries, int capacity, ObjString* key) {
    uint32_t index = key->hash % capacity;

    // Probe until you find the entry or a bucket that can contain the new entry. Guaranteed to find at least one empty
    // bucket because max load factor is 75%
    for (;;) {
        Entry* entry = &(entries[index]);

        if (entry->key == key || entry->key == NULL) {
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

/**
    Adjust the capacity of a hash Table and sort existing Entries into new buckets based on new capacity
 */
static void adjustCapacity(Table* table, int capacity) {
    Entry* entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NIL_VAL;
    }

    // Copy over any existing entries in old entries array. Remap them to the appropriate bucket in new array
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &(table->entries[i]);
        if (entry->key == NULL) continue;

        Entry* dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

/**
    Get an Entry by key, and put its Value into *value*. Returns true if found an entry and false otherwise
 */
bool tableGet(Table* table, ObjString* key, Value* value) {
    if (table->entries == NULL) return false;

    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) return false;

    *value = entry->value;
    return true;
}

/**
    Set an entry in a table
 */
bool tableSet(Table* table, ObjString* key, Value value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    Entry* entry = findEntry(table->entries, table->capacity, key);

    bool isNewKey = entry->key == NULL;
    if (isNewKey) table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

/**
    Copy over the contents of one table to another
 */
void tableAddAll(Table* from, Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &(from->entries[i]);
        if (entry->key != NULL) {
            tableSet(to, entry->key, entry->value);
        }
    }
}