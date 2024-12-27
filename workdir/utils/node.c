#include "node.h"


node_t* create_node(node_value_t data, bool is_operator) {
    node_t* node = (node_t*) malloc(sizeof(node_t));

    node->data = data;
    node->is_operator = is_operator;
    node->left = NULL;
    node->right = NULL;

    return node;
}


void destroy_node(node_t* node) {
    if (node->left)
        destroy_node(node->left);
    if (node->right)
        destroy_node(node->right);

    free(node);
}


void append_left_child(node_t* parent, node_t* left_child) {
    if (!(parent->is_operator))
        return;

    parent->left = left_child;
}

void append_right_child(node_t* parent, node_t* right_child) {
    if (!(parent->is_operator))
        return;

    parent->right = right_child;
}

int evaluate_node(node_t* node) {
    if (!node)
        return 0;

    if (!(node->is_operator))
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

    node->is_operator
        ? printf("%c ", (node->data).c_val)
        : printf("%d ", (node->data).n_val);

    print_prefix(node->left);
    print_prefix(node->right);
}

void print_postfix(node_t* node) {
    if (!node)
        return;

    print_postfix(node->left);
    print_postfix(node->right);

    node->is_operator
        ? printf("%c ", (node->data).c_val)
        : printf("%d ", (node->data).n_val);
}