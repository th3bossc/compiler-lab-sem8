#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdbool.h>
#include <string.h>
#include "../node/ast_node.h"
#include "../instr_set/instr_set.h"
#include "../symbol_table/var_details.h"
#include "../activation_record/activation_record.h"


void generate_function_code(ast_node_t* node);
void generate_class_code();
void generate_class_method_code(class_method_t* method);
void generate_program_structure(ast_node_t* node);
void generate_statement_structure(ast_node_t* node, label_index_t* break_label, label_index_t* continue_label);

reg_index_t generate_arithmetic_code(ast_node_t* node);
reg_index_t generate_boolean_code(ast_node_t* node);
reg_index_t generate_string_code(ast_node_t* node);
reg_index_t generate_arr_index_code(ast_node_t* node);
reg_index_t generate_expression_code(ast_node_t* node);
reg_index_t generate_ptr_deref_code(ast_node_t* node);
reg_index_t generate_ptr_ref_code(ast_node_t* node);
reg_index_t generate_id_expr_code(ast_node_t* node);
reg_index_t generate_func_call_code(ast_node_t* node);
reg_index_t generate_tuple_index_code(ast_node_t* node);
reg_index_t generate_init_heap_code();
reg_index_t generate_alloc_code();
reg_index_t generate_free_heap_code(ast_node_t* node);
// reg_index_t generate_self_field_code(ast_node_t* node);
// reg_index_t generate_self_method_code(ast_node_t* node);
reg_index_t generate_class_method_call_code(ast_node_t* node);
reg_index_t generate_class_constructor_code(ast_node_t* node);

void generate_assignment_code(ast_node_t* node);
void generate_arr_assignment_code(ast_node_t* node);
void generate_ptr_assignment_code(ast_node_t* node);
void generate_tuple_field_assignment_code(ast_node_t* node);
// void generate_self_field_assignment_code(ast_node_t* node);

reg_index_t generate_print_code(ast_node_t* node);
reg_index_t generate_read_code(ast_node_t* node);

void generate_if_code(ast_node_t* node, label_index_t* break_label, label_index_t* continue_label);
void generate_ifelse_code(ast_node_t* node, label_index_t* break_label, label_index_t* continue_label);
void generate_while_code(ast_node_t* node);
void generate_do_while_code(ast_node_t* node);
void generate_repeat_code(ast_node_t* node);
void generate_func_return_code(ast_node_t* node);

reg_index_t print_register(reg_index_t data);
reg_index_t print_addr(int addr);
reg_index_t read_into_addr(int addr);
reg_index_t read_into_reg_addr(reg_index_t reg_index);

reg_index_t initialize_heap_code();
reg_index_t alloc_memory(int size);
reg_index_t free_memory(reg_index_t location);


void exit_program(int* free_registers);
void generate_headers();

void generate_program(ast_node_t* body);

#endif