#include "instr_set.h"

void register_addressing(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "MOV R%d, R%d\n", r1, r2);
}

void immediate_addressing_int(reg_index_t r1, int data, FILE* fp) {
    fprintf(fp, "MOV R%d, %d\n", r1, data);
}

void immediate_addressing_str(reg_index_t r1, char* data, FILE* fp) {
    fprintf(fp, "MOV R%d, %s\n", r1, data);
}

void register_indirect_addressing_store(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "MOV [R%d], R%d\n", r1, r2);
}

void register_indirect_addressing_load(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "MOV R%d, [R%d]\n", r1, r2);
}

void direct_addressing_store(int address, reg_index_t r1, FILE* fp) {
    fprintf(fp, "MOV [%d], R%d\n", address, r1);
}

void direct_addressing_load(int address, reg_index_t r1, FILE* fp) {
    fprintf(fp, "MOV R%d, [%d]\n", r1, address);
}




void add_registers(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "ADD R%d, R%d\n", r1, r2);
}

void sub_registers(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "SUB R%d, R%d\n", r1, r2);
}

void mul_registers(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "MUL R%d, R%d\n", r1, r2);
}

void div_registers(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "DIV R%d, R%d\n", r1, r2);
}

void mod_registers(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "MOD R%d, R%d\n", r1, r2);
}

void add_immediate(reg_index_t r1, int data, FILE* fp) {
    fprintf(fp, "ADD R%d, %d\n", r1, data);
}

void sub_immediate(reg_index_t r1, int data, FILE* fp) {
    fprintf(fp, "SUB R%d, %d\n", r1, data);
}
void mul_immediate(reg_index_t r1, int data, FILE* fp) {
    fprintf(fp, "MUL R%d, %d\n", r1, data);
}
void div_immediate(reg_index_t r1, int data, FILE* fp) {
    fprintf(fp, "DIV R%d, %d\n", r1, data);
}
void mod_immediate(reg_index_t r1, int data, FILE* fp) {
    fprintf(fp, "MOD R%d, %d\n", r1, data);
}

void incr_register(reg_index_t r1, FILE* fp) {
    fprintf(fp, "INR R%d\n", r1);
}

void decr_register(reg_index_t r1, FILE* fp) {
    fprintf(fp, "DCR R%d\n", r1);
}



void less_than(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "LT R%d, R%d\n", r1, r2);
}

void greater_than(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "GT R%d, R%d\n", r1, r2);
}

void equals(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "EQ R%d, R%d\n", r1, r2);
}

void not_equals(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "NE R%d, R%d\n", r1, r2);
}

void greater_than_or_equals(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "GE R%d, R%d\n", r1, r2);
}

void less_than_or_equals(reg_index_t r1, reg_index_t r2, FILE* fp) {
    fprintf(fp, "LE R%d, R%d\n", r1, r2);
}





void jump(int address, FILE* fp) {
    fprintf(fp, "JMP %d\n", address);
}

void jump_to_label(label_index_t label, FILE* fp) {
    fprintf(fp, "JMP L%d\n", label);
}

void jump_zero(reg_index_t r1, int address, FILE* fp) {
    fprintf(fp, "JZ R%d, %d\n", r1, address);
}

void jump_zero_to_label(reg_index_t r1, label_index_t label, FILE* fp) {
    fprintf(fp, "JZ R%d, L%d\n", r1, label);
}

void jump_not_zero(reg_index_t r1, int address, FILE* fp) {
    fprintf(fp, "JNZ R%d, %d\n", r1, address);
}

void jump_not_zero_to_label(reg_index_t r1, label_index_t label, FILE* fp) {
    fprintf(fp, "JNZ R%d, L%d\n", r1, label);
}

void add_label(label_index_t label, FILE* fp) {
    fprintf(fp, "L%d:\n", label);
}


void push_register(reg_index_t r1, FILE* fp) {
    fprintf(fp, "PUSH R%d\n", r1);
}
void pop_register(reg_index_t r1, FILE* fp) {
    fprintf(fp, "POP R%d\n", r1);
}




void call_function(int address, FILE* fp) {
    fprintf(fp, "CALL %d\n", address);
}

void return_from_function(FILE* fp) {
    fprintf(fp, "RET\n");
}



void add_breakpoint(FILE* fp) {
    fprintf(fp, "BRKP\n");
}




void call_library_function(reg_index_t func_name, reg_index_t arg1, reg_index_t arg2, reg_index_t arg3, reg_index_t ret_val, FILE* fp) {
    push_register(func_name, fp);
    push_register(arg1, fp);
    push_register(arg2, fp);
    push_register(arg3, fp);
    push_register(ret_val, fp);
    call_function(0, fp);
}

void post_library_call(reg_index_t ret_val, reg_index_t free_reg, FILE* fp) {
    pop_register(ret_val, fp);
    pop_register(free_reg, fp);
    pop_register(free_reg, fp);
    pop_register(free_reg, fp);
    pop_register(free_reg, fp);
}



void reset_registers() {
    for (reg_index_t i = 0; i < NUM_REGISTERS; i++)
        free_registers[i] = true;
}


void reset_labels() {
    used_labels = 0;
}


void free_register(reg_index_t reg) {
    free_registers[reg] = true;
}

label_index_t get_label() {
    used_labels++;
    return used_labels;
}

reg_index_t get_free_register() {
    for (reg_index_t i = 0; i < NUM_REGISTERS; i++) {
        if (free_registers[i] == true) {
            free_registers[i] = false;
            return i;
        }
    }

    perror("Out of registers");
    exit(1);
}
