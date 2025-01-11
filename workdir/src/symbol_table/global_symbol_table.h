#ifndef GLOBAL_SYMBOL_TABLE_H
#define GLOBAL_SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../type_table/type_table.h"
#include "../node/decl_node.h"

typedef struct global_symbol_table_s global_symbol_table_t;


// forward decl for ast_node
typedef struct ast_node_s ast_node_t;

struct global_symbol_table_s {
    char* name;
    type_table_t* type;
    type_table_t* inner_type;
    int size;
    int inner_size;
    int binding;
    decl_node_t* params;
    ast_node_t* func_body;
    decl_node_t* local_decls;
    global_symbol_table_t* next;
};

global_symbol_table_t* global_symbol_table;
int free_address;

int get_binding(int size);
global_symbol_table_t* create_global_symbol_table_entry(char* name, type_table_t* type, int size);
global_symbol_table_t* create_global_symbol_table_array_entry(char* name, type_table_t* inner_type, int outer_size, int inner_size);
global_symbol_table_t* create_global_symbol_table_pointer_entry(char* name, type_table_t* type_table, int size);
global_symbol_table_t* create_global_symbol_table_func_entry(char* name, type_table_t* return_type, decl_node_t* params_list);
global_symbol_table_t* global_symbol_table_lookup(char* name);

void print_symbol_table();
void create_global_table();
void destroy_global_table();


#endif