#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NODE_H
#define NODE_H

typedef struct node_s node_t;
typedef union node_value_s node_value_t;

union node_value_s {
    int n_val;
    char c_val;
};


struct node_s {
    node_value_t data;
    bool is_operator;

    node_t* left;
    node_t* right;
};



node_t* create_node(node_value_t data, bool is_leaf);
void destroy_node(node_t* node);
void append_left_child(node_t* parent, node_t* left_child);
void append_right_child(node_t* parent, node_t* right_child);
int evaluate_node(node_t* node);
void print_prefix(node_t* node);
void print_postfix(node_t* node);

#endif