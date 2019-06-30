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
    Entry* tombstone = NULL;

    // Probe until you find the entry or a bucket that can contain the new entry. Guaranteed to find entry or an empty
    // bucket because max load factor is 75%. If you come across a bucket that has a NULL key, check if its a tombstone
    // by checking its value, and if so, save the tombstone entry. IF you come across a truly empty bucket, return the
    // tombstone or NULL (we return the tombstone so it can be reused for setting key/values). Tombstones are considered
    // to be full entries for load factor purposes
    for (;;) {
        Entry* entry = &(entries[index]);

        if (entry->key == NULL) {
            if (IS_NIL(entry->value)) {
                // Empty entry
                return tombstone != NULL ? tombstone : entry;
            } else {
                // We found a tombstone
                if (tombstone == NULL) tombstone = entry;
            }
        } else if (entry->key == key) {
            // We found the key
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

    table->count = 0;  // Don't carry over tombstones
    // Copy over any existing entries in old entries array. Remap them to the appropriate bucket in new array
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &(table->entries[i]);
        if (entry->key == NULL) continue;

        Entry* dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
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
    if (isNewKey && IS_NIL(entry->value)) table->count++;  // Increment count if the key is new and not going into a tombstone

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

/**
    Delete a value by replacing it with a tombstone (empty key, value of true). Tombstones mark a spot in the Table that
    has no real value, but should not trigger the end of the probe (since we're using open addressing to store values)
 */
bool tableDelete(Table* table, ObjString* key) {
    if (table->count == 0) return false;

    // Find the entry
    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) return false;

    entry->key = NULL;
    entry->value = BOOL_VAL(true);

    return true;
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