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

bool verify_params_list(decl_node_t* params_list1, decl_node_t* params_list2) {
    decl_node_t* it1 = params_list1;
    decl_node_t* it2 = params_list2;

    while(it1 != NULL && it2 != NULL) {
        if (strcmp(it1->name, it2->name) != 0)
            return false;
        if (it1->type != it2->type)
            return false; 
    }

    if (it1 != NULL || it2 != NULL) 
        return false;

    return true;
}

void destroy_decl_node(decl_node_t* head) {
    if (head->next != NULL) {
        destroy_decl_node(head->next);
    }

    free(head);
}