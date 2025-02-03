#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../symbol_table/global_symbol_table.h"
#include "../symbol_table/local_symbol_table.h"
#include "args_node.h"
#include "../instr_set/instr_set.h"
#include "class_decl_node.h"
typedef enum node_type_s node_type_t;
typedef struct ast_node_s ast_node_t;
typedef union node_value_s node_value_t;

//forward decl for yyerror & get_label
void yyerror(const char*);

enum node_type_s {
    NODE_TYPE_WRITE,
    NODE_TYPE_READ,
    NODE_TYPE_INIT_HEAP,
    NODE_TYPE_ALLOC,
    NODE_TYPE_FREE,
    NODE_TYPE_CONNECTOR,
    NODE_TYPE_ID,
    NODE_TYPE_TUPLE_FIELD,
    NODE_TYPE_INT,
    NODE_TYPE_STRING,
    NODE_TYPE_ASSIGN,
    NODE_TYPE_OPERATOR,
    NODE_TYPE_RELOP,
    NODE_TYPE_IF,
    NODE_TYPE_IFELSE,
    NODE_TYPE_WHILE,
    NODE_TYPE_DO_WHILE,
    NODE_TYPE_REPEAT,
    NODE_TYPE_BREAK,
    NODE_TYPE_CONTINUE,
    NODE_TYPE_ARR_INDEX,
    NODE_TYPE_PTR_REF,
    NODE_TYPE_PTR_DEREF,
    NODE_TYPE_FUNC_CALL,
    NODE_TYPE_FUNC_DECL,
    NODE_TYPE_PROGRAM,
    NODE_TYPE_FUNC_RET,
};


union node_value_s {
    int n_val;
    char c_val;
    char* s_val;
};


struct ast_node_s {
    node_value_t data;
    node_type_t node_type;
    global_symbol_table_t* func_details;
    args_node_t* args_list;
    type_table_t* value_type;
    ast_node_t* left;
    ast_node_t* right;
    ast_node_t* middle;
};



ast_node_t* create_num_node(int val);
ast_node_t* create_string_node(char* data);
ast_node_t* create_id_node(char* var_name);
ast_node_t* create_tuple_field_node(ast_node_t* addr_node, char* field_name);
ast_node_t* create_operator_node(char op, ast_node_t* left, ast_node_t* right);
ast_node_t* create_write_node(ast_node_t* expr);
ast_node_t* create_read_node(ast_node_t* expr);
ast_node_t* create_init_heap_node();
ast_node_t* create_alloc_node();
ast_node_t* create_free_node(char* var_name);
ast_node_t* create_arr_index_node(char* arr_name, ast_node_t* index_node);
ast_node_t* create_2d_arr_index_node(char* arr_name, ast_node_t* outer_index_node, ast_node_t* inner_index_node);
ast_node_t* create_ptr_deref_node(ast_node_t* expr);
ast_node_t* create_ptr_ref_node(ast_node_t* expr);
ast_node_t* create_connector_node(ast_node_t* left, ast_node_t* right);
ast_node_t* create_program_node(ast_node_t* left, ast_node_t* right);
ast_node_t* create_assignment_node(char* var_name, ast_node_t* expr);
ast_node_t* create_arr_assignment_node(char* var_name, ast_node_t* index_node, ast_node_t* expr);
ast_node_t* create_2d_arr_assignment_node(char* var_name, ast_node_t* outer_index_node, ast_node_t* inner_index_node, ast_node_t* expr);
ast_node_t* create_ptr_assignment_node(char* var_name, ast_node_t* expr);
ast_node_t* create_tuple_field_assignment_node(ast_node_t* addr_node, char* field_name, ast_node_t* expr);
ast_node_t* create_ifelse_node(ast_node_t* condn, ast_node_t* if_subtree, ast_node_t* else_subtree);
ast_node_t* create_while_node(ast_node_t* condn, ast_node_t* body);
ast_node_t* create_do_while_node(ast_node_t* condn, ast_node_t* body);
ast_node_t* create_repeat_node(ast_node_t* condn, ast_node_t* body);
ast_node_t* create_relop_node(char relop[], ast_node_t* left, ast_node_t* right);
ast_node_t* create_func_call_node(char* func_name, args_node_t* args_list);
ast_node_t* create_func_body_node(char* func_name, type_table_t* return_type, decl_node_t* params_list, decl_node_t* local_decls, ast_node_t* body);
ast_node_t* create_func_return_node(ast_node_t* expr);
ast_node_t* create_break_node();
ast_node_t* create_continue_node();

void destroy_node(ast_node_t* node);
// void evaluate_node(ast_node_t* node);
void print_prefix(ast_node_t* node);
void print_postfix(ast_node_t* node);

#endif