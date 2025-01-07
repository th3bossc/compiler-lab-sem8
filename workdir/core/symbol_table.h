#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct symbol_table_s symbol_table_t;
typedef enum symbol_type_s symbol_type_t;


enum symbol_type_s {
    SYMBOL_TYPE_INT,
    SYMBOL_TYPE_STR,
    SYMBOL_TYPE_BOOL,
    SYMBOL_TYPE_VOID,
    SYMBOL_TYPE_NOT_SET,
    SYMBOL_TYPE_ARR,
    SYMBOL_TYPE_PTR,
};

struct symbol_table_s {
    char* name;
    symbol_type_t type;
    int size;
    int inner_size;
    symbol_type_t inner_type;
    int binding;
    symbol_table_t* next;   
};

symbol_table_t* symbol_table;
int free_address;

int get_binding(int size);
symbol_table_t* create_symbol_table_entry(char* name, symbol_type_t type, int size);
symbol_table_t* create_symbol_table_array_entry(char* name, symbol_type_t inner_type, int outer_size, int inner_size);
symbol_table_t* create_symbol_table_pointer_entry(char* name, symbol_type_t inner_type, int size);
symbol_table_t* symbol_table_lookup(char* name);
// symbol_table_t* find_var_at_address(int address);
void print_symbol_table();
void create_table();
void destroy_table();


#endif