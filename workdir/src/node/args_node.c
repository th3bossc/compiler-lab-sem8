#include "args_node.h"



args_node_t* create_args_node(ast_node_t* expr_node) {
    args_node_t* node = (args_node_t*) malloc(sizeof(args_node_t));
    node->expr_node = expr_node;
    node->next = NULL;

    return node;
}

args_node_t* join_args_nodes(args_node_t* node1, args_node_t* node2) {
    if (node1 == NULL)
        return node2;
    if (node2 == NULL)
        return node1;

    args_node_t* tail = node1;
    while(tail->next != NULL)
        tail = tail->next;

    tail->next = node2;
    return node1;
}

int count_num_args(args_node_t* args) {
    args_node_t* it = args;
    int count = 0;
    while(it != NULL) {
        count++;
        it = it->next;
    }

    return count;
}


void destroy_args_node(args_node_t* node) {
    if (node->next != NULL) {
        destroy_args_node(node->next);
    }

    free(node->expr_node);
    free(node);
}