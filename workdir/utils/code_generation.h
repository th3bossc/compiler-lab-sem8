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
reg_index_t generate_arithmetic_code(node_t* node, FILE* target_file);
void reset_registers();
int print_content(reg_index_t data, FILE* target_file);
void exit_program(FILE* target_file);
void generate_headers(FILE* target_file);

#endif