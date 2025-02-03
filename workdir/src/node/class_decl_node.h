#ifndef CLASS_DECL_NODE_H
#define CLASS_DECL_NODE_H

#include <stdlib.h>
#include "../type_table/type_table.h"

typedef struct class_decl_node_s class_decl_node_t;
typedef enum class_decl_node_type_s class_decl_node_type_t;

typedef struct ast_node_s ast_node_t;
int get_label();

enum class_decl_node_type_s {
    CLASS_DECL_NODE_TYPE_FIELD,
    CLASS_DECL_NODE_TYPE_METHOD,
};

struct class_decl_node_s {
    class_decl_node_type_t node_type;
    char* name;
    type_table_t* type;
    int label;
    decl_node_t* params;
    ast_node_t* func_body;
    decl_node_t* local_decls;
    class_decl_node_t* next;
};


class_decl_node_t* create_class_decl_field_node(char* name, type_table_t* type);
class_decl_node_t* create_class_decl_method_node(char* name, type_table_t* return_type, decl_node_t* params, ast_node_t* func_body, decl_node_t* local_decls);
void destroy_class_decl_node(class_decl_node_t* node);
class_decl_node_t* join_class_decl_nodes(class_decl_node_t* node1, class_decl_node_t* node2);


#endif