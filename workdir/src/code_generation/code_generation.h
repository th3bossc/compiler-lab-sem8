#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdbool.h>
#include <string.h>
#include "../node/ast_node.h"
#include "../instr_set/instr_set.h"
#include "../type_table/type_table.h"
#include "../symbol_table/var_details.h"
#include "../class_table/class_table.h"



void generate_function_code(ast_node_t* node);
void generate_class_code();
void generate_class_method_code(class_method_t* method);
void generate_program_structure(ast_node_t* node);
void generate_statement_structure(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table, label_index_t* break_label, label_index_t* continue_label, global_symbol_table_t* func_entry, class_method_t* method_entry);

reg_index_t generate_arithmetic_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_boolean_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_string_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_arr_index_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_expression_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_ptr_deref_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_ptr_ref_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_id_expr_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_func_call_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_tuple_index_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_init_heap_code(int* num_used_regs);
reg_index_t generate_alloc_code(int* num_used_regs);
reg_index_t generate_free_heap_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);


void generate_assignment_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
void generate_arr_assignment_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
void generate_ptr_assignment_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
void generate_tuple_field_assignment_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);


reg_index_t generate_print_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);
reg_index_t generate_read_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table);

void generate_if_code(ast_node_t* node, int* num_used_regs, label_index_t* break_label, label_index_t* continue_label, local_symbol_table_t* l_symbol_table, global_symbol_table_t* func_entry, class_method_t* method_entry);
void generate_ifelse_code(ast_node_t* node, int* num_used_regs, label_index_t* break_label, label_index_t* continue_label, local_symbol_table_t* l_symbol_table, global_symbol_table_t* func_entry, class_method_t* method_entry);
void generate_while_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table, global_symbol_table_t* func_entry, class_method_t* method_entry);
void generate_do_while_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table, global_symbol_table_t* func_entry, class_method_t* method_entry);
void generate_repeat_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table, global_symbol_table_t* func_entry, class_method_t* method_entry);
void generate_func_return_code(ast_node_t* node, int* num_used_regs, local_symbol_table_t* l_symbol_table, global_symbol_table_t* func_entry, class_method_t* method_entry);

reg_index_t print_register(reg_index_t data, int* num_used_regs);
reg_index_t print_addr(int addr, int* num_used_regs);
reg_index_t read_into_addr(int addr, int* num_used_regs);
reg_index_t read_into_reg_addr(reg_index_t reg_index, int* num_used_regs);

reg_index_t initialize_heap_code(int* num_used_regs);
reg_index_t alloc_memory(int size, int* num_used_regs);
reg_index_t free_memory(reg_index_t location, int* num_used_regs);


void exit_program(int* num_used_regs);
void generate_headers();

void generate_program(ast_node_t* body);

#endif