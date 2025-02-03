#ifndef CLASS_TABLE_H
#define CLASS_TABLE_H

#include <stdlib.h>
#include <string.h>
#include "../node/class_decl_node.h"
#include "member_field_list.h"
#include "member_func_list.h"

typedef struct class_table_s class_table_t;

struct class_table_s {
    char* name;
    class_field_t* fields;
    class_method_t* methods;
    class_table_t* parent_class;
    int num_fields;
    int num_methods;
    class_table_t* next;
};

class_table_t* class_table;

class_table_t* create_class_table_entry(char* name, class_decl_node_t* decl_list, class_decl_node_t* method_list, class_table_t* parent_class);
void destroy_class_table_entry(class_table_t* entry);
class_table_t* class_table_lookup(char* name);

void initialize_class_table();


#endif