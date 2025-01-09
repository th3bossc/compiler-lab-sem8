#include <stdlib.h>
#include <string.h>

#ifndef TYPE_TABLE_H
#define TYPE_TABLE_H


typedef struct type_table_s type_table_t;
typedef struct field_list_s field_list_t;
typedef struct primitive_types_s primitive_types_t;

struct field_list_s {
    char* name;
    int field_index;
    type_table_t* type;
    field_list_t* next;
};


struct type_table_s {
    char* name;
    int size;
    field_list_t* fields;
    int num_fields;
    type_table_t* next;
};


struct primitive_types_s {
    type_table_t* int_type;
    type_table_t* ptr_type;
    type_table_t* arr_type;
    type_table_t* str_type;
    type_table_t* void_type;
    type_table_t* bool_type;
    type_table_t* unset_type;
};

type_table_t* type_table;
primitive_types_t* default_types;

field_list_t* create_field_list_entry(char* name, type_table_t* struct_name, type_table_t* field_type);
void destroy_field_list_entry(field_list_t* entry);


type_table_t* create_type_table_entry(char* name, int size);
void destroy_type_table_entry(type_table_t* entry);

type_table_t* get_type_table_entry(char* name);
field_list_t* get_field_entry(type_table_t* struct_name, int field_index);

void initialize_type_table();

#endif