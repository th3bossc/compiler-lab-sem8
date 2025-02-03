#ifndef MEM_FIELD_LIST_H
#define MEM_FIELD_LIST_H

#include <stdlib.h>
#include "../type_table/type_table.h"


typedef struct class_field_s class_field_t;

// forward decls
void yyerror(const char*);
typedef struct class_table_s class_table_t;

struct class_field_s {
    char* name;
    int field_index;
    type_table_t* type;
    // class_table_t* class_type;
    class_field_t* next;
};


class_field_t* create_class_field_entry(char* name, type_table_t* type);
void destroy_class_field_entry(class_field_t* entry);
class_field_t* class_field_lookup(class_table_t* class_name, char* field_name);

#endif