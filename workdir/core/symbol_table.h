#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct symbol_table_s symbol_table_t;
typedef enum symbol_type_s symbol_type_t;


enum symbol_type_s {
    NODE_VALUE_INT,
    NODE_VALUE_STR,
    NODE_VALUE_BOOL,
    NODE_VALUE_VOID,
    NODE_VALUE_NOT_SET,
};

struct symbol_table_s {
    char* name;
    symbol_type_t type;
    int size;
    int binding;
    symbol_table_t* next;   
};

symbol_table_t* symbol_table;
int free_address;

int get_binding(int size);
symbol_table_t* create_symbol_table_entry(char* name, symbol_type_t type, int size);
symbol_table_t* symbol_table_lookup(char* name);
void print_symbol_table();
void create_table();
void destroy_table();


#endif