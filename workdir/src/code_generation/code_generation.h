#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdbool.h>
#include <string.h>
#include "../node/ast_node.h"
#include "../instr_set/instr_set.h"
#include "../type_table/type_table.h"
#include "../symbol_table/var_details.h"



void generate_function_code(ast_node_t* node, FILE* target_file);
void generate_program_structure(ast_node_t* node, FILE* target_file);
void generate_statement_structure(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table, label_index_t* break_label, label_index_t* continue_label);

reg_index_t generate_arithmetic_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_boolean_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_string_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_arr_index_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_expression_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_ptr_deref_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_ptr_ref_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_id_expr_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);

void generate_assignment_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
void generate_arr_assignment_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
void generate_ptr_assignment_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);

reg_index_t generate_print_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_read_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);

void generate_if_code(ast_node_t* node, FILE* target_file, int* num_used_regs, label_index_t* break_label, label_index_t* continue_label, local_symbol_table_t* l_symbol_table);
void generate_ifelse_code(ast_node_t* node, FILE* target_file, int* num_used_regs, label_index_t* break_label, label_index_t* continue_label, local_symbol_table_t* l_symbol_table);
void generate_while_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
void generate_do_while_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);
void generate_repeat_code(ast_node_t* node, FILE* target_file, int* num_used_regs, local_symbol_table_t* l_symbol_table);

reg_index_t print_register(reg_index_t data, FILE* target_file, int* num_used_regs);
reg_index_t print_addr(int addr, FILE* target_file, int* num_used_regs);
reg_index_t read_into_addr(int addr, FILE* target_file, int* num_used_regs);
reg_index_t read_into_reg_addr(reg_index_t reg_index, FILE* target_file, int* num_used_regs);

void exit_program(FILE* target_file, int* num_used_regs);
void generate_headers(FILE* target_file);

void generate_program(ast_node_t* body, FILE* target_file);

#endif