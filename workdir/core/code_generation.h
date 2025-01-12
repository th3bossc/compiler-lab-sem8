#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdbool.h>
#include "node.h"
#include "instr_set.h"





reg_index_t generate_arithmetic_code(node_t* node, FILE* target_file);
reg_index_t generate_boolean_code(node_t* node, FILE* target_file);
reg_index_t generate_string_code(node_t* node, FILE* target_file);
reg_index_t generate_arr_index_code(node_t* node, FILE* target_file);
reg_index_t generate_expression_code(node_t* node, FILE* target_file);
reg_index_t generate_ptr_deref_code(node_t* node, FILE* target_file);
reg_index_t generate_ptr_ref_code(node_t* node, FILE* target_file);

void generate_assignment_code(node_t* node, FILE* target_file);
void generate_arr_assignment_code(node_t* node, FILE* target_file);
void generate_ptr_assignment_code(node_t* node, FILE* target_file);
reg_index_t generate_print_code(node_t* node, FILE* target_file);
reg_index_t generate_read_code(node_t* node, FILE* target_file);
void generate_statement_structure(node_t* node, FILE* target_file, label_index_t* break_label, label_index_t* continue_label);
void generate_if_code(node_t* node, FILE* target_file, label_index_t* break_label, label_index_t* continue_label);
void generate_ifelse_code(node_t* node, FILE* target_file, label_index_t* break_label, label_index_t* continue_label);
void generate_while_code(node_t* node, FILE* target_file);
void generate_do_while_code(node_t* node, FILE* target_file);
void generate_repeat_code(node_t* node, FILE* target_file);
reg_index_t print_register(reg_index_t data, FILE* target_file);
reg_index_t print_addr(int addr, FILE* target_file);
reg_index_t read_into_addr(int addr, FILE* target_file);
reg_index_t read_into_reg_addr(reg_index_t reg_index, FILE* target_file);

void exit_program(FILE* target_file);
void generate_headers(FILE* target_file);

void generate_program(node_t* body, FILE* target_file);

#endif