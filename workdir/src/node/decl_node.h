#ifndef DECL_NODE_H
#define DECL_NODE_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct decl_node_s decl_node_t;
typedef struct type_table_s type_table_t;

struct decl_node_s {
    char* name;
    char* undeclared_type;
    type_table_t* type;
    type_table_t* inner_type;
    decl_node_t* next;
};

decl_node_t* create_decl_node(char* name, type_table_t* type, type_table_t* inner_type);
decl_node_t* create_user_type_decl_node(char* name, char* type);
decl_node_t* join_decl_nodes(decl_node_t* node1, decl_node_t* node2);
bool verify_params_list(decl_node_t* params_list1, decl_node_t* params_list2);
void destroy_decl_node(decl_node_t* head);


#endif