#include "decl_node.h"

decl_node_t* create_decl_node(char* name, type_table_t* type) {
    decl_node_t* node = (decl_node_t*) malloc(sizeof(decl_node_t));

    node->name = strdup(name);
    node->type = type;
    node->next = NULL;

    return node;
}

decl_node_t* join_decl_nodes(decl_node_t* node1, decl_node_t* node2) {
    if (node1 == NULL)
        return node2;
    else if (node2 == NULL)
        return node1;

    decl_node_t* tail = node1;
    while (tail->next != NULL)
        tail = tail->next;

    tail->next = node2;
    return node1;
}

void destroy_decl_node(decl_node_t* head) {
    if (head->next != NULL) {
        destroy_decl_node(head->next);
    }

    free(head);
}