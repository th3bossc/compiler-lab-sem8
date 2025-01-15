#ifndef VAR_DETAILS_H
#define VAR_DETAILS_H

#include <stdlib.h>
#include "global_symbol_table.h"
#include "local_symbol_table.h"

typedef struct symbol_table_entry_s symbol_table_entry_t;
typedef union table_entry_s table_entry_t;
typedef enum table_entry_type_s table_entry_type_t;

//forwaard decl for yyerror
void yyerror(const char*);


enum table_entry_type_s {
    ENTRY_TYPE_GLOBAL,
    ENTRY_TYPE_LOCAL,
    ENTRY_NOT_FOUND,
};

union table_entry_s {
    global_symbol_table_t* global_entry;
    local_symbol_table_t* local_entry;
};


struct symbol_table_entry_s {
    table_entry_t entry;
    table_entry_type_t entry_type;
};


symbol_table_entry_t get_var_details(char* name, local_symbol_table_t* local_table);
type_table_t* get_var_type(char* name, local_symbol_table_t* local_table);
type_table_t* get_var_inner_type(char* name, local_symbol_table_t* local_table);
#endif