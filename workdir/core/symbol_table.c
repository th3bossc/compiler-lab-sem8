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

symbol_table_t* create_entry(char* name, symbol_type_t type, int size, int inner_size, symbol_type_t inner_type, int binding, symbol_table_t* next) {
    symbol_table_t* entry = (symbol_table_t*) malloc(sizeof(symbol_table_t));
    entry->name = strdup(name);
    entry->type = type;
    entry->size = size;
    entry->inner_size = inner_size;
    entry->inner_type = inner_type;
    entry->binding = binding;
    entry->next = next;

    return entry;
}


symbol_table_t* create_symbol_table_entry(char* name, symbol_type_t type, int size) {
    symbol_table_t* entry = create_entry(
        name,
        type,
        size,
        0,
        SYMBOL_TYPE_VOID,
        get_binding(size),
        NULL
    );


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

symbol_table_t* create_symbol_table_array_entry(char* name, symbol_type_t inner_type, int outer_size, int inner_size) {
    symbol_table_t* entry = create_entry(
        name,
        SYMBOL_TYPE_ARR,
        outer_size,
        inner_size,
        inner_type,
        get_binding(outer_size*inner_size),
        NULL
    );

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

symbol_table_t* create_symbol_table_pointer_entry(char* name, symbol_type_t inner_type, int size) {
    symbol_table_t* entry = create_entry(
        name,
        SYMBOL_TYPE_PTR,
        size,
        0,
        inner_type,
        get_binding(size),
        NULL
    );

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
        if (entry->type == SYMBOL_TYPE_INT) {
            type = "INT";
        }
        else if (entry->type == SYMBOL_TYPE_STR) {
            type = "STR";
        }
        else if (entry->type == SYMBOL_TYPE_ARR) {
            if (entry->inner_type == SYMBOL_TYPE_INT) {
                type = "ARR[INT]";
            }
            else if (entry->inner_size == SYMBOL_TYPE_STR) {
                type = "ARR[STR]";
            }
        }
        else if (entry->type == SYMBOL_TYPE_PTR) {
            if (entry->inner_type == SYMBOL_TYPE_INT) {
                type = "PTR[INT]";
            }
            else if (entry->inner_type == SYMBOL_TYPE_STR) {
                type = "PTR[STR]";
            }
        }
        printf("{ name: %s, type: %s, size: %d, content_size: %d }\n", entry->name, type, entry->size, entry->inner_size);
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