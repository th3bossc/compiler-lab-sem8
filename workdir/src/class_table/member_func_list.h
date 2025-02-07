#ifndef MEM_FUNC_LIST_H
#define MEM_FUNC_LIST_H



#include <stdlib.h>
#include "../type_table/type_table.h"
#include "../instr_set/instr_set.h"


typedef struct class_method_s class_method_t;

// forward decls
void yyerror(const char*);
typedef struct class_table_s class_table_t;

struct class_method_s {
    char* name;
    class_table_t* class;
    type_table_t* return_type;
    int method_index;
    decl_node_t* params;
    decl_node_t* local_decls;
    ast_node_t* func_body;
    label_index_t label;
    class_method_t* next;
};


class_method_t* create_class_method_entry(class_table_t* class_details, char* name, type_table_t* return_type, decl_node_t* params, ast_node_t* func_body, decl_node_t* local_decls, label_index_t label, int method_index);
void destroy_class_method_entry(class_method_t* entry);
class_method_t* class_method_lookup(class_table_t* class_name, char* method_name);

#endif