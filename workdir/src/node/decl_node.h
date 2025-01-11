#ifndef DECL_NODE_H
#define DECL_NODE_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../type_table/type_table.h"

typedef struct decl_node_s decl_node_t;


struct decl_node_s {
    char* name;
    type_table_t* type;
    decl_node_t* next;
};

decl_node_t* create_decl_node(char* name, type_table_t* type);
decl_node_t* join_decl_nodes(decl_node_t* node1, decl_node_t* node2);
bool verify_params_list(decl_node_t* params_list1, decl_node_t* params_list2);
void destroy_decl_node(decl_node_t* head);


#endif