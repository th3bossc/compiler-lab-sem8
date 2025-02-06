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
#define RESERVED_RETURN_REG 0


FILE* instr_set_fp;


//forward decls
typedef struct symbol_table_entry_s symbol_table_entry_t;

// util funcs
label_index_t used_labels;

reg_index_t get_free_register(int* num_used_regs);
void free_used_register(int* num_used_regs, reg_index_t reg);
void reset_registers(int* num_used_regs);
void reset_labels();
label_index_t get_label();



// data transfer instrs
void register_addressing(reg_index_t r1, reg_index_t r2);                     // MOV R1, R2
void immediate_addressing_int(reg_index_t r1, int data);                      // MOV R1, 5
void immediate_addressing_str(reg_index_t r1, char* data);                    // MOV R1, "data"
void register_indirect_addressing_store(reg_index_t r1, reg_index_t r2);      // MOV [R1], R2
void register_indirect_addressing_load(reg_index_t r1, reg_index_t r2);       // MOV R1, [R2]
void direct_addressing_store(int address, reg_index_t r1);                    // MOV [LOC], R1
void direct_addressing_load(int address, reg_index_t r1);                     // MOV R1, [LOC]


// arithmetic instrs
void add_registers(reg_index_t r1, reg_index_t r2);   // ADD R1, R2
void sub_registers(reg_index_t r1, reg_index_t r2);   // SUB R1, R2
void mul_registers(reg_index_t r1, reg_index_t r2);   // MUL R1, R2
void div_registers(reg_index_t r1, reg_index_t r2);   // DIV R1, R2
void mod_registers(reg_index_t r1, reg_index_t r2);   // MOD R1, R2
void add_immediate(reg_index_t r1, int data);         // ADD R1, 1
void sub_immediate(reg_index_t r1, int data);         // SUB R1, 1
void mul_immediate(reg_index_t r1, int data);         // MUL R1, 1
void div_immediate(reg_index_t r1, int data);         // DIV R1, 1
void mod_immediate(reg_index_t r1, int data);         // MOD R1, 1
void incr_register(reg_index_t r1);                   // INR R1
void decr_register(reg_index_t r1);                   // DCR R1


// logical instructions
void less_than(reg_index_t r1, reg_index_t r2);                   // LT R1, R2
void greater_than(reg_index_t r1, reg_index_t r2);                // GT R1, R2
void equals(reg_index_t r1, reg_index_t r2);                      // EQ R1, R2
void not_equals(reg_index_t r1, reg_index_t r2);                  // NE R1, R2
void greater_than_or_equals(reg_index_t r1, reg_index_t r2);      // GE R1, R2
void less_than_or_equals(reg_index_t r1, reg_index_t r2);         // LE R1, R2


// branching instrs
void jump(int address);                                               // JMP 0
void jump_zero(reg_index_t r1, int address);                          // JZ R1, 0
void jump_not_zero(reg_index_t r1, int address);                      // JNZ R1, 0
void jump_to_label(label_index_t label);                              // JMP L1
void jump_not_zero_to_label(reg_index_t r1, label_index_t label);     // JNZ R1, L1 
void jump_zero_to_label(reg_index_t r1, label_index_t label);         // JZ R1, L1
void add_label(label_index_t label);


// stack instrs
void push_register(reg_index_t r1);   // PUSH R1
void pop_register(reg_index_t r1);    // POP R1


// subroutine instrs
void call_function(int address);          // CALL 0
void return_from_function();                // RET


// breakpoint
void add_breakpoint();          // BRKP




// compound instrs
void call_library_function(reg_index_t func_name, reg_index_t arg1, reg_index_t arg2, reg_index_t arg3, reg_index_t ret_val, int* num_used_regs);
void post_library_call(reg_index_t ret_val, reg_index_t free_reg, int* num_used_regs);
void save_machine_state(int* num_used_regs);
void restore_machine_state(int* num_used_regs);

#endif