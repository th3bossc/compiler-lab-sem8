#include "decl_node.h"
#include "ast_node.h"
decl_node_t* create_decl_node(char* name, type_table_t* type, type_table_t* inner_type) {
    decl_node_t* node = (decl_node_t*) malloc(sizeof(decl_node_t));

    node->name = strdup(name);
    node->type = type;
    node->inner_type = inner_type;
    node->next = NULL;

    return node;
}


decl_node_t* create_user_type_decl_node(char* name, char* type) {
    decl_node_t* node = (decl_node_t*) malloc(sizeof(decl_node_t));

    node->name = strdup(name);
    node->undeclared_type = strdup(type);
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
        if (it1->inner_type != it2->inner_type)
            return false;

        it1 = it1->next;
        it2 = it2->next;
    }

    if (it1 != NULL || it2 != NULL) 
        return false;

    return true;
}

bool verify_args_match_params(decl_node_t* params, args_node_t* args) {
    decl_node_t* decl_it = params;
    args_node_t* args_it = args;
    while(decl_it != NULL && args_it != NULL) {
        if (decl_it->type != args_it->expr_node->value_type) {
            return false;
        }

        decl_it = decl_it->next;
        args_it = args_it->next;
    }

    if (decl_it != NULL || args_it != NULL)
        return false;

    return true;
}

int count_num_params(decl_node_t* params) {
    decl_node_t* it = params;
    int count = 0;
    while(it != NULL) {
        count++;
        it = it->next;
    }

    return count;
}

void destroy_decl_node(decl_node_t* head) {
    if (head->next != NULL) {
        destroy_decl_node(head->next);
    }

    free(head);
}