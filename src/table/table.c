#include <stdlib.h>
#include <string.h>

#include "../memory/memory.h"
#include "../object/object.h"
#include "table.h"
#include "../value/value.h"

void initTable(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void freeTable(Table* table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}