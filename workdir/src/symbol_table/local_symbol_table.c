#include "local_symbol_table.h"


local_symbol_table_t* create_local_symbol_table_entry(char* name, type_table_t* type, int binding, local_symbol_table_t* next) {
    local_symbol_table_t* entry = (local_symbol_table_t*) malloc(sizeof(local_symbol_table_t));

    entry->name = strdup(name);
    entry->type = type;
    entry->binding = binding;
    entry->next = next;

    return entry;
}


void destroy_local_symbol_table(local_symbol_table_t* table) {
    if (table->next) {
        destroy_local_symbol_table(table->next);
    } 
    free(table->name);
    free(table);
}


local_symbol_table_t* local_symbol_table_lookup(local_symbol_table_t* table, char* name) {
    for (local_symbol_table_t* entry = table; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) 
            return entry;
    }

    return NULL;
}


local_symbol_table_t* append_to_local_table(local_symbol_table_t* head, local_symbol_table_t* new_item) {
    local_symbol_table_t* tail = head;
    if (tail == NULL) {
        return new_item;
    }
    while(tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = new_item;
    return head;
}