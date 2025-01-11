#ifndef ARGS_NODE_H
#define ARGS_NODE_H

#include <stdlib.h>
#include <string.h>


typedef struct args_node_s args_node_t;


//forward decl for ast_node
typedef struct ast_node_s ast_node_t;

struct args_node_s {
    ast_node_t* expr_node;
    args_node_t* next;
};


args_node_t* create_args_node(ast_node_t* expr_node);
args_node_t* join_args_nodes(args_node_t* node1, args_node_t* node2);
void destroy_args_node(args_node_t* node);



#endif