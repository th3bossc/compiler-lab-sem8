#include "node.h"

node_t* create_node(node_value_t data, node_type_t type, node_t* left, node_t* right) {
    node_t* node = (node_t*) malloc(sizeof(node_t));
    node->data = data;
    node->node_type = type;
    node->left = left;
    node->right = right;

    return node;
}


node_t* create_num_node(int val) {
    node_value_t data;
    data.n_val = val;

    node_t* node = create_node(data, NODE_TYPE_VALUE, NULL, NULL);
    return node;
}

node_t* create_id_node(char var_name) {
    node_value_t data;
    data.var_name = var_name;

    node_t* node = create_node(data, NODE_TYPE_ID, NULL, NULL);
    return node;
}

node_t* create_operator_node(char op, node_t* left, node_t* right) {
    node_value_t data;
    data.c_val = op;

    node_t* node = create_node(data, NODE_TYPE_OPERATOR, left, right);
    return node;
} 

node_t* create_write_node(node_t* expr) {
    node_value_t data;
    node_t* node = create_node(data, NODE_TYPE_WRITE, expr, NULL);
    return node;
}

node_t* create_read_node(char var_name) {
    node_t* var_node = create_id_node(var_name);
    node_value_t data;
    node_t* node = create_node(data, NODE_TYPE_READ, var_node, NULL);
    return node;
}

node_t* create_connector_node(node_t* left, node_t* right) {
    node_value_t data;
    node_t* node = create_node(data, NODE_TYPE_CONNECTOR, left, right);
    return node;
}

node_t* create_assignment_node(char var_name, node_t* expr) {
    node_value_t data;
    data.c_val = '=';
    node_t* id_node = create_id_node(var_name);
    node_t* node = create_node(data, NODE_TYPE_ASSIGN, id_node, expr);
    return node;
}


void destroy_node(node_t* node) {
    if (node->left)
        destroy_node(node->left);
    if (node->right)
        destroy_node(node->right);

    free(node);
}


int evaluate_node(node_t* node) {
    if (!node)
        return 0;

    if (node->node_type == NODE_TYPE_VALUE)
        return (node->data).n_val;

    char op = (node->data).c_val;
    int l_value = evaluate_node(node->left);
    int r_value = evaluate_node(node->right);
    int answer = 0;
    switch (op) {
        case '+':
            answer = l_value + r_value;
            break;
        case '-':
            answer = l_value - r_value;
            break;
        case '*':
            answer =  l_value * r_value;
            break;
        case '/':
            answer = l_value / r_value;
            break;
    }


    return answer;
}

void print_prefix(node_t* node) {
    if (!node)
        return;

    switch (node->node_type) {
        case NODE_TYPE_CONNECTOR:
            printf("CONNECTOR ");
            break;
        case NODE_TYPE_WRITE:
            printf("WRITE ");
            break;
        case NODE_TYPE_READ:
            printf("READ ");
            break;
        case NODE_TYPE_ID:
            printf("%c ", node->data.var_name);
            break;
        case NODE_TYPE_VALUE:
            printf("%d ", node->data.n_val);
            break;
        case NODE_TYPE_ASSIGN:
        case NODE_TYPE_OPERATOR:
            printf("%c ", node->data.c_val);
            break;
    }

    print_prefix(node->left);
    print_prefix(node->right);
}

void print_postfix(node_t* node) {
    if (!node)
        return;

    print_postfix(node->left);
    print_postfix(node->right);

    switch (node->node_type) {
        case NODE_TYPE_CONNECTOR:
            printf("CONNECTOR ");
            break;
        case NODE_TYPE_WRITE:
            printf("WRITE ");
            break;
        case NODE_TYPE_READ:
            printf("READ ");
            break;
        case NODE_TYPE_ID:
            printf("%c ", node->data.var_name);
            break;
        case NODE_TYPE_VALUE:
            printf("%d ", node->data.n_val);
            break;
        case NODE_TYPE_ASSIGN:
        case NODE_TYPE_OPERATOR:
            printf("%c ", node->data.c_val);
            break;
    }
}