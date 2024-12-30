#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum node_type_s node_type_t;
typedef enum node_value_type_s node_value_type_t;
typedef struct node_s node_t;
typedef union node_value_s node_value_t;

enum node_type_s {
    NODE_TYPE_WRITE,
    NODE_TYPE_READ,
    NODE_TYPE_CONNECTOR,
    NODE_TYPE_OPERATOR,
    NODE_TYPE_RELOP,
    NODE_TYPE_VALUE,
    NODE_TYPE_ID,
    NODE_TYPE_ASSIGN,
    NODE_TYPE_IF,
    NODE_TYPE_IFELSE,
    NODE_TYPE_WHILE,
    NODE_TYPE_REPEAT,
    NODE_TYPE_DO_WHILE,
    NODE_TYPE_BREAK,
    NODE_TYPE_CONTINUE,
};

enum node_value_type_s {
    NODE_VALUE_INT,
    NODE_VALUE_BOOL,
    NODE_VALUE_VOID,
    NODE_VALUE_NOT_SET,
};

union node_value_s {
    int val;
    char op;
    char relop[2];
    char var_name;
};


struct node_s {
    node_value_t data;
    node_type_t node_type;
    node_value_type_t value_type;
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
node_t* create_ifelse_node(node_t* condn, node_t* if_subtree, node_t* else_subtree);
node_t* create_while_node(node_t* condn, node_t* body);
node_t* create_do_while_node(node_t* condn, node_t* body);
node_t* create_repeat_node(node_t* condn, node_t* body);
node_t* create_relop_node(char relop[], node_t* left, node_t* right);
node_t* create_break_node();
node_t* create_continue_node();

int get_addr(char var_name);
void destroy_node(node_t* node);
void evaluate_node(node_t* node);
void print_prefix(node_t* node);
void print_postfix(node_t* node);

#endif