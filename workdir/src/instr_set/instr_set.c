#include "instr_set.h"

void register_addressing(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "MOV R%d, R%d\n", r1, r2);
}

void immediate_addressing_int(reg_index_t r1, int data) {
    fprintf(instr_set_fp, "MOV R%d, %d\n", r1, data);
}

void immediate_addressing_str(reg_index_t r1, char* data) {
    fprintf(instr_set_fp, "MOV R%d, %s\n", r1, data);
}

void register_indirect_addressing_store(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "MOV [R%d], R%d\n", r1, r2);
}

void register_indirect_addressing_load(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "MOV R%d, [R%d]\n", r1, r2);
}

void direct_addressing_store(int address, reg_index_t r1) {
    fprintf(instr_set_fp, "MOV [%d], R%d\n", address, r1);
}

void direct_addressing_load(int address, reg_index_t r1) {
    fprintf(instr_set_fp, "MOV R%d, [%d]\n", r1, address);
}




void add_registers(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "ADD R%d, R%d\n", r1, r2);
}

void sub_registers(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "SUB R%d, R%d\n", r1, r2);
}

void mul_registers(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "MUL R%d, R%d\n", r1, r2);
}

void div_registers(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "DIV R%d, R%d\n", r1, r2);
}

void mod_registers(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "MOD R%d, R%d\n", r1, r2);
}

void add_immediate(reg_index_t r1, int data) {
    fprintf(instr_set_fp, "ADD R%d, %d\n", r1, data);
}

void sub_immediate(reg_index_t r1, int data) {
    fprintf(instr_set_fp, "SUB R%d, %d\n", r1, data);
}
void mul_immediate(reg_index_t r1, int data) {
    fprintf(instr_set_fp, "MUL R%d, %d\n", r1, data);
}
void div_immediate(reg_index_t r1, int data) {
    fprintf(instr_set_fp, "DIV R%d, %d\n", r1, data);
}
void mod_immediate(reg_index_t r1, int data) {
    fprintf(instr_set_fp, "MOD R%d, %d\n", r1, data);
}

void incr_register(reg_index_t r1) {
    fprintf(instr_set_fp, "INR R%d\n", r1);
}

void decr_register(reg_index_t r1) {
    fprintf(instr_set_fp, "DCR R%d\n", r1);
}



void less_than(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "LT R%d, R%d\n", r1, r2);
}

void greater_than(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "GT R%d, R%d\n", r1, r2);
}

void equals(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "EQ R%d, R%d\n", r1, r2);
}

void not_equals(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "NE R%d, R%d\n", r1, r2);
}

void greater_than_or_equals(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "GE R%d, R%d\n", r1, r2);
}

void less_than_or_equals(reg_index_t r1, reg_index_t r2) {
    fprintf(instr_set_fp, "LE R%d, R%d\n", r1, r2);
}





void jump(int address) {
    fprintf(instr_set_fp, "JMP %d\n", address);
}

void jump_to_label(label_index_t label) {
    fprintf(instr_set_fp, "JMP L%d\n", label);
}

void jump_zero(reg_index_t r1, int address) {
    fprintf(instr_set_fp, "JZ R%d, %d\n", r1, address);
}

void jump_zero_to_label(reg_index_t r1, label_index_t label) {
    fprintf(instr_set_fp, "JZ R%d, L%d\n", r1, label);
}

void jump_not_zero(reg_index_t r1, int address) {
    fprintf(instr_set_fp, "JNZ R%d, %d\n", r1, address);
}

void jump_not_zero_to_label(reg_index_t r1, label_index_t label) {
    fprintf(instr_set_fp, "JNZ R%d, L%d\n", r1, label);
}

void add_label(label_index_t label) {
    fprintf(instr_set_fp, "L%d:\n", label);
}


void push_register(reg_index_t r1) {
    fprintf(instr_set_fp, "PUSH R%d\n", r1);
}
void pop_register(reg_index_t r1) {
    fprintf(instr_set_fp, "POP R%d\n", r1);
}




void call_function(int address) {
    fprintf(instr_set_fp, "CALL %d\n", address);
}

void return_from_function() {
    fprintf(instr_set_fp, "RET\n");
}



void add_breakpoint() {
    fprintf(instr_set_fp, "BRKP\n");
}




void call_library_function(reg_index_t func_name, reg_index_t arg1, reg_index_t arg2, reg_index_t arg3, reg_index_t ret_val, int* num_used_regs) {
    save_machine_state(num_used_regs);
    push_register(func_name);
    push_register(arg1);
    push_register(arg2);
    push_register(arg3);
    push_register(ret_val);
    call_function(0);
}

void post_library_call(reg_index_t ret_val, reg_index_t free_reg, int* num_used_regs) {
    pop_register(ret_val);
    pop_register(free_reg);
    pop_register(free_reg);
    pop_register(free_reg);
    pop_register(free_reg);
    restore_machine_state(num_used_regs);
}

void save_machine_state(int* num_used_regs) {
    for (int i = 1; i < NUM_REGISTERS; i++) {
        if (num_used_regs[i] == false) {
            push_register(i);
        }
    }
}

void restore_machine_state(int* num_used_regs) {
    for (int i = NUM_REGISTERS-1; i > 0; i--) {
        if (num_used_regs[i] == false) {
            pop_register(i);
        }
    }
}



void reset_registers(int* num_used_regs) {
    for (int i = 1; i < NUM_REGISTERS; i++) {
        num_used_regs[i] = true;
    }
}   


void reset_labels() {
    used_labels = 0;
}


void free_used_register(int* num_used_regs, reg_index_t reg) {
    if (reg <= 0)
        return;
    num_used_regs[reg] = true;
}

label_index_t get_label() {
    used_labels++;
    return used_labels;
}

reg_index_t get_free_register(int* num_used_regs) {\
    reg_index_t ret_val = -1;
    for (reg_index_t i = 1; i < NUM_REGISTERS; i++) {
        if (num_used_regs[i] == true) {
            num_used_regs[i] = false;
            ret_val = i;
            break;
        }
    }
    if (ret_val == -1) {
        yyerror("Out of registers");
        exit(1);
    }
    
    return ret_val;
}
