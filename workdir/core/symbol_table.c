#include "symbol_table.h"


symbol_table_t* symbol_table_tail;

void create_table() {
    symbol_table = NULL;
    symbol_table_tail = NULL;
    free_address = 4096;
}

int get_binding(int size) {
    int start_address = free_address;
    free_address += size;
    return start_address;
}


symbol_table_t* create_symbol_table_entry(char* name, symbol_type_t type, int size) {
    symbol_table_t* entry = (symbol_table_t*) malloc(sizeof(symbol_table_t));
    entry->name = strdup(name);
    entry->type = type;
    entry->size = size;    
    entry->binding = get_binding(size);
    entry->next = NULL;

    if (!symbol_table) {
        symbol_table = entry;
        symbol_table_tail = entry;
    }
    else {
        symbol_table_tail->next = entry;
        symbol_table_tail = entry;
    }

    return entry;
}

symbol_table_t* symbol_table_lookup(char* name) {
    for (symbol_table_t* entry = symbol_table; entry != NULL; entry = entry->next) {
        if (strcmp(name, entry->name) == 0) {
            return entry;
        }
    }
    return NULL;
}

void print_symbol_table() {
    for (symbol_table_t* entry = symbol_table; entry != NULL; entry = entry->next) {
        char* type;
        if (entry->type == NODE_VALUE_INT)
            type = "INT";
        else if (entry->type == NODE_VALUE_STR)
            type = "STR";
        printf("{ name: %s, type: %s, size: %d }\n", entry->name, type, entry->size);
    }
}


void destroy_table_entry(symbol_table_t* table_entry) {
    if (!table_entry)
        return;
    free(table_entry->name);
    destroy_table_entry(table_entry->next);
    free(table_entry);
}

void destroy_table() {
    destroy_table_entry(symbol_table);
}