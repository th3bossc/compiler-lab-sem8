#include "class_decl_node.h"

class_decl_node_t* create_class_decl_field_node(char* name, char* type) {
    class_decl_node_t* entry = (class_decl_node_t*) malloc(sizeof(class_decl_node_t));

    entry->node_type = CLASS_DECL_NODE_TYPE_FIELD;
    entry->name = strdup(name);
    // entry->type = type;
    entry->undeclared_type = strdup(type);
    entry->func_body = NULL;
    entry->label = -1;
    entry->params = NULL;
    entry->local_decls = NULL;
    entry->next = NULL;

    return entry;
}

class_decl_node_t* create_class_decl_method_node(char* name, char* return_type, decl_node_t* params, ast_node_t* func_body, decl_node_t* local_decls) {
    class_decl_node_t* entry = (class_decl_node_t*) malloc(sizeof(class_decl_node_t));

    entry->node_type = CLASS_DECL_NODE_TYPE_METHOD;
    entry->name = strdup(name);
    entry->undeclared_type = return_type;
    entry->func_body = func_body;
    entry->label = get_label();
    entry->params = params;
    entry->local_decls = local_decls;
    entry->next = NULL;

    return entry;
}

void destroy_class_decl_node(class_decl_node_t* node) {
    if (node == NULL)
        return;

    free(node->name);
    free(node->func_body);
    free(node->params);
    free(node->local_decls);

    destroy_class_decl_node(node->next);

    free(node);
}


class_decl_node_t* join_class_decl_nodes(class_decl_node_t* node1, class_decl_node_t* node2) {
    class_decl_node_t* tail = node1;
    while(tail->next != NULL)
        tail = tail->next;

    tail->next = node2;

    return node1;
}
