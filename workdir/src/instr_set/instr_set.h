#ifndef INSTR_SET_H
#define INSTR_SET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../node/ast_node.h"

#define reg_index_t int
#define label_index_t int
#define NUM_REGISTERS 20
#define ERR_NO_REGS -1

// util funcs
bool free_registers[NUM_REGISTERS];
label_index_t used_labels;

reg_index_t get_free_register();
void free_register(reg_index_t reg);
void reset_registers();
void reset_labels();
label_index_t get_label();



// data transfer instrs
void register_addressing(reg_index_t r1, reg_index_t r2, FILE* fp);                     // MOV R1, R2
void immediate_addressing_int(reg_index_t r1, int data, FILE* fp);                      // MOV R1, 5
void immediate_addressing_str(reg_index_t r1, char* data, FILE* fp);                    // MOV R1, "data"
void register_indirect_addressing_store(reg_index_t r1, reg_index_t r2, FILE* fp);      // MOV [R1], R2
void register_indirect_addressing_load(reg_index_t r1, reg_index_t r2, FILE* fp);       // MOV R1, [R2]
void direct_addressing_store(int address, reg_index_t r1, FILE* fp);                    // MOV [LOC], R1
void direct_addressing_load(int address, reg_index_t r1, FILE* fp);                     // MOV R1, [LOC]


// arithmetic instrs
void add_registers(reg_index_t r1, reg_index_t r2, FILE* fp);   // ADD R1, R2
void sub_registers(reg_index_t r1, reg_index_t r2, FILE* fp);   // SUB R1, R2
void mul_registers(reg_index_t r1, reg_index_t r2, FILE* fp);   // MUL R1, R2
void div_registers(reg_index_t r1, reg_index_t r2, FILE* fp);   // DIV R1, R2
void mod_registers(reg_index_t r1, reg_index_t r2, FILE* fp);   // MOD R1, R2
void add_immediate(reg_index_t r1, int data, FILE* fp);         // ADD R1, 1
void sub_immediate(reg_index_t r1, int data, FILE* fp);         // SUB R1, 1
void mul_immediate(reg_index_t r1, int data, FILE* fp);         // MUL R1, 1
void div_immediate(reg_index_t r1, int data, FILE* fp);         // DIV R1, 1
void mod_immediate(reg_index_t r1, int data, FILE* fp);         // MOD R1, 1
void incr_register(reg_index_t r1, FILE* fp);                   // INR R1
void decr_register(reg_index_t r1, FILE* fp);                   // DCR R1


// logical instructions
void less_than(reg_index_t r1, reg_index_t r2, FILE* fp);                   // LT R1, R2
void greater_than(reg_index_t r1, reg_index_t r2, FILE* fp);                // GT R1, R2
void equals(reg_index_t r1, reg_index_t r2, FILE* fp);                      // EQ R1, R2
void not_equals(reg_index_t r1, reg_index_t r2, FILE* fp);                  // NE R1, R2
void greater_than_or_equals(reg_index_t r1, reg_index_t r2, FILE* fp);      // GE R1, R2
void less_than_or_equals(reg_index_t r1, reg_index_t r2, FILE* fp);         // LE R1, R2


// branching instrs
void jump(int address, FILE* fp);                                               // JMP 0
void jump_zero(reg_index_t r1, int address, FILE* fp);                          // JZ R1, 0
void jump_not_zero(reg_index_t r1, int address, FILE* fp);                      // JNZ R1, 0
void jump_to_label(label_index_t label, FILE* fp);                              // JMP L1
void jump_not_zero_to_label(reg_index_t r1, label_index_t label, FILE* fp);     // JNZ R1, L1 
void jump_zero_to_label(reg_index_t r1, label_index_t label, FILE* fp);         // JZ R1, L1
void add_label(label_index_t label, FILE* fp);


// stack instrs
void push_register(reg_index_t r1, FILE* fp);   // PUSH R1
void pop_register(reg_index_t r1, FILE* fp);    // POP R1


// subroutine instrs
void call_function(int address, FILE* fp);          // CALL 0
void return_from_function(FILE* fp);                // RET


// breakpoint
void add_breakpoint(FILE* fp);          // BRKP




// compound instrs
void call_library_function(reg_index_t func_name, reg_index_t arg1, reg_index_t arg2, reg_index_t arg3, reg_index_t ret_val, FILE* fp);
void post_library_call(reg_index_t ret_val, reg_index_t free_reg, FILE* fp);

#endif