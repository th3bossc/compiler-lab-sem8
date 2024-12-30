#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdbool.h>
#include "node.h"

#define NUM_REGISTERS 20
#define ERR_NO_REGS -1
#define reg_index_t int



bool free_registers[NUM_REGISTERS];
reg_index_t get_free_register();
void free_register(reg_index_t reg);
void reset_registers();

reg_index_t generate_arithmetic_code(node_t* node, FILE* target_file);
void generate_assignment_code(node_t* node, FILE* target_file);
reg_index_t generate_print_code(node_t* node, FILE* target_file);
reg_index_t generate_read_code(node_t* node, FILE* target_file);
void generate_statement_structure(node_t* node, FILE* target_file);


void load_addr_to_register(reg_index_t reg, int addr, FILE* target_file);
void store_register_to_addr(reg_index_t reg, int addr, FILE* target_file);
reg_index_t print_register(reg_index_t data, FILE* target_file);
reg_index_t print_addr(int addr, FILE* target_file);
reg_index_t read_into_addr(int addr, FILE* target_file);

void exit_program(FILE* target_file);
void generate_headers(FILE* target_file);

#endif