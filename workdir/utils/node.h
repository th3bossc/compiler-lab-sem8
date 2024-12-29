#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NODE_H
#define NODE_H

typedef struct node_s node_t;
typedef union node_value_s node_value_t;
typedef enum node_type_s node_type_t;


enum node_type_s {
    NODE_TYPE_WRITE,
    NODE_TYPE_READ,
    NODE_TYPE_CONNECTOR,
    NODE_TYPE_OPERATOR,
    NODE_TYPE_VALUE,
    NODE_TYPE_ID,
    NODE_TYPE_ASSIGN,
};

union node_value_s {
    int n_val;
    char c_val;
    char var_name;
};


struct node_s {
    node_value_t data;
    node_type_t node_type;

    node_t* left;
    node_t* right;
};



// node_t* create_node(node_value_t data, node_type_t node_type, node_t* left, node_t* right);
node_t* create_num_node(int val);
node_t* create_id_node(char var_name);
node_t* create_operator_node(char op, node_t* left, node_t* right);
node_t* create_write_node(node_t* expr);
node_t* create_read_node(char var_name);
node_t* create_connector_node(node_t* left, node_t* right);
node_t* create_assignment_node(char var_name, node_t* expr);

int get_addr(char var_name);
void destroy_node(node_t* node);
void evaluate_node(node_t* node);
void print_prefix(node_t* node);
void print_postfix(node_t* node);

#endif