#include "var_details.h"


symbol_table_entry_t get_var_details(char* name, local_symbol_table_t* local_table) {
    symbol_table_entry_t entry;
    local_symbol_table_t* local_entry = local_symbol_table_lookup(local_table, name);
    if (local_entry) {
        entry.entry.local_entry = local_entry;
        entry.entry_type = ENTRY_TYPE_LOCAL;
    }
    else {
        global_symbol_table_t* global_entry = global_symbol_table_lookup(name);
        if (global_entry) {
            entry.entry.global_entry = global_entry;
            entry.entry_type = ENTRY_TYPE_GLOBAL;
        }
        else {
            entry.entry_type = ENTRY_NOT_FOUND;
        }
    }

    return entry;
}


type_table_t* get_var_type(char* name, local_symbol_table_t* local_table) {
    symbol_table_entry_t entry = get_var_details(name, local_table);

    if (entry.entry_type == ENTRY_NOT_FOUND) {
        yyerror("{var_details:get_var_type} Variable not declared");
        exit(1);
    }


    if (entry.entry_type == ENTRY_TYPE_GLOBAL)
        return entry.entry.global_entry->type;
    if (entry.entry_type == ENTRY_TYPE_LOCAL)
        return entry.entry.local_entry->type;
    
}

type_table_t* get_var_inner_type(char* name, local_symbol_table_t* local_table) {
    symbol_table_entry_t entry = get_var_details(name, local_table);

    if (entry.entry_type == ENTRY_NOT_FOUND) {
        yyerror("{var_details:get_var_inner_type} Variable not declared");
        exit(1);
    }


    if (entry.entry_type == ENTRY_TYPE_GLOBAL)
        return entry.entry.global_entry->inner_type;
    if (entry.entry_type == ENTRY_TYPE_LOCAL)
        return entry.entry.local_entry->inner_type;
}

global_symbol_table_t* get_var_at_address(int address) {
    global_symbol_table_t* it = global_symbol_table;
    while(it != NULL) {
        if (it->binding == address) 
            return it;
        it = it->next;
    }

    return NULL;
}