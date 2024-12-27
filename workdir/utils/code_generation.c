#include "./code_generation.h"

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


void free_register(reg_index_t reg) {
    free_registers[reg] = true;
}

reg_index_t generate_arithmetic_code(node_t* node, FILE* target_file) {
    if (!node)
        return -1;

    if (node->is_operator == false) {
        reg_index_t free_reg = get_free_register();

        fprintf(target_file, "MOV R%d, %d\n", free_reg, (node->data).n_val);
        return free_reg;
    }

    char op = (node->data).c_val;
    reg_index_t l_val_reg = generate_arithmetic_code(node->left, target_file);
    reg_index_t r_val_reg = generate_arithmetic_code(node->right, target_file);
    printf("op: %d\n", op);
    switch (op) {
        case '+':
            fprintf(target_file, "ADD R%d, R%d\n", l_val_reg, r_val_reg);
            break;
        case '-':
            fprintf(target_file, "SUB R%d, R%d\n", l_val_reg, r_val_reg);
            break;
        case '*':
            fprintf(target_file, "MUL R%d, R%d\n", l_val_reg, r_val_reg);
            break;
        case '/':
            fprintf(target_file, "DIV R%d, R%d\n", l_val_reg, r_val_reg);
            break;
        default: 
            perror("unknown operator");
            exit(1);
    }
    free_register(r_val_reg);
    return l_val_reg;
}


void reset_registers() {
    for (reg_index_t i = 0; i < NUM_REGISTERS; i++)
        free_registers[i] = true;
}


reg_index_t print_content(reg_index_t data, FILE* target_file) {
    fprintf(target_file, "\n\n");
    reg_index_t free_reg = get_free_register();

    fprintf(target_file, "MOV R%d, \"Write\"\nPUSH R%d\n", free_reg, free_reg);
    fprintf(target_file, "MOV R%d, -2\nPUSH R%d\n", free_reg, free_reg);
    fprintf(target_file, "PUSH R%d\n", data);
    fprintf(target_file, "PUSH R%d\n", free_reg);
    fprintf(target_file, "PUSH R%d\n", free_reg);
    fprintf(target_file, "CALL 0\n");


    reg_index_t ret_val = get_free_register();
    fprintf(target_file, "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n", ret_val, free_reg, free_reg, free_reg, free_reg);

    free_register(free_reg);
    fprintf(target_file, "\n\n");
}

void exit_program(FILE* target_file) {
    fprintf(target_file, "\n\n");
    reg_index_t free_reg = get_free_register();

    fprintf(target_file, "MOV R%d, \"Exit\"\nPUSH R%d\n", free_reg, free_reg);
    fprintf(target_file, "PUSH R%d\n", free_reg);
    fprintf(target_file, "PUSH R%d\n", free_reg);
    fprintf(target_file, "PUSH R%d\n", free_reg);
    fprintf(target_file, "PUSH R%d\n", free_reg);
    fprintf(target_file, "CALL 0\n");


    reg_index_t ret_val = get_free_register();
    fprintf(target_file, "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n", ret_val, free_reg, free_reg, free_reg, free_reg);

    free_register(free_reg);
    fprintf(target_file, "\n\n");
}

void generate_headers(FILE* target_file) {
    fprintf(target_file, "0\n2056\n0\n0\n0\n0\n0\n0\n\n");
}