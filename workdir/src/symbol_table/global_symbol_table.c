#include "global_symbol_table.h"


global_symbol_table_t* global_symbol_table_tail;

void create_global_table() {
    global_symbol_table = NULL;
    global_symbol_table_tail = NULL;
    free_address = 4096;

    global_symbol_table_t* main_node = create_global_symbol_table_func_entry("main", default_types->int_type, NULL);
}

int get_binding(int size) {
    int start_address = free_address;
    free_address += size;
    return start_address;
}

global_symbol_table_t* create_entry(char* name, type_table_t* type, int size, int inner_size, type_table_t* inner_type, int binding, global_symbol_table_t* next) {
    global_symbol_table_t* entry = (global_symbol_table_t*) malloc(sizeof(global_symbol_table_t));
    entry->name = strdup(name);
    entry->type = type;
    entry->size = size;
    entry->inner_size = inner_size;
    entry->inner_type = inner_type;
    entry->binding = binding;
    entry->next = next;

    return entry;
}


global_symbol_table_t* create_global_symbol_table_entry(char* name, type_table_t* type, int size) {
    if (!type) {
        perror("{global_symbol_table:create_global_symbol_table_entry} type not found");
        exit(1);
    }
    global_symbol_table_t* entry = create_entry(
        name,
        type,
        size,
        0,
        NULL,
        get_binding(size),
        NULL
    );


    if (!global_symbol_table) {
        global_symbol_table = entry;
        global_symbol_table_tail = entry;
    }
    else {
        global_symbol_table_tail->next = entry;
        global_symbol_table_tail = entry;
    }

    return entry;
}

global_symbol_table_t* create_global_symbol_table_array_entry(char* name, type_table_t* inner_type, int outer_size, int inner_size) {
    type_table_t* type = get_type_table_entry("arr");
    if (!type || !inner_type) {
        perror("{global_symbol_table:create_global_symbol_table_array_entry} type not found");
        exit(1);
    }

    global_symbol_table_t* entry = create_entry(
        name,
        type,
        outer_size,
        inner_size,
        inner_type,
        get_binding(outer_size*inner_size),
        NULL
    );

    if (!global_symbol_table) {
        global_symbol_table = entry;
        global_symbol_table_tail = entry;
    }
    else {
        global_symbol_table_tail->next = entry;
        global_symbol_table_tail = entry;
    }

    return entry;
}

global_symbol_table_t* create_global_symbol_table_pointer_entry(char* name, type_table_t* inner_type, int size) {
    type_table_t* type = get_type_table_entry("ptr");
    if (!type || !inner_type) {
        perror("{global_symbol_table:create_global_symbol_table_pointer_entry} type not found");
        exit(1);
    }

    global_symbol_table_t* entry = create_entry(
        name,
        type,
        size,
        0,
        inner_type,
        get_binding(size),
        NULL
    );

    if (!global_symbol_table) {
        global_symbol_table = entry;
        global_symbol_table_tail = entry;
    }
    else {
        global_symbol_table_tail->next = entry;
        global_symbol_table_tail = entry;
    }

    return entry;
}

global_symbol_table_t* create_global_symbol_table_func_entry(char* name, type_table_t* return_type, decl_node_t* params_list) {
    type_table_t* func_type = default_types->func_type;
    global_symbol_table_t* entry = create_entry(
        name,
        func_type,
        0,
        0,
        return_type,
        get_binding(0),
        NULL
    );
    entry->params = params_list;
    if (!global_symbol_table) {
        global_symbol_table = entry;
        global_symbol_table_tail = entry;
    }
    else {
        global_symbol_table_tail->next = entry;
        global_symbol_table_tail = entry;
    }

    return entry;
}


global_symbol_table_t* global_symbol_table_lookup(char* name) {
    for (global_symbol_table_t* entry = global_symbol_table; entry != NULL; entry = entry->next) {
        if (strcmp(name, entry->name) == 0) {
            return entry;
        }
    }
    return NULL;
}

void print_symbol_table() {
    for (global_symbol_table_t* entry = global_symbol_table; entry != NULL; entry = entry->next) {
        printf("{ name: %s, type: %s, size: %d, content_size: %d }\n", entry->name, entry->type->name, entry->size, entry->inner_size);
    }
}


void destroy_table_entry(global_symbol_table_t* table_entry) {
    if (!table_entry)
        return;
    free(table_entry->name);
    destroy_table_entry(table_entry->next);
    free(table_entry);
}

void destroy_global_table() {
    destroy_table_entry(global_symbol_table);
}