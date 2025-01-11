#ifndef LOCAL_SYMBOL_TABLE_H
#define LOCAL_SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include "../type_table/type_table.h"

typedef struct local_symbol_table_s local_symbol_table_t;

struct local_symbol_table_s {
    char* name;
    type_table_t* type;
    local_symbol_table_t* next;
    int binding;
};


local_symbol_table_t* create_local_symbol_table_entry(char* name, type_table_t* type, int binding, local_symbol_table_t* next);
void destroy_local_symbol_table(local_symbol_table_t* table);
local_symbol_table_t* append_to_local_table(local_symbol_table_t* head, local_symbol_table_t* new_item);
local_symbol_table_t* local_symbol_table_lookup(local_symbol_table_t* table, char* name);



#endif