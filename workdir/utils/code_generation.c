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

int get_addr(char var_name) {
    return (4096 + var_name - 'a');
}

reg_index_t generate_arithmetic_code(node_t* node, FILE* target_file) {
    if (!node)
        return -1;

    if (node->node_type == NODE_TYPE_VALUE) {
        reg_index_t free_reg = get_free_register();

        fprintf(target_file, "MOV R%d, %d\n", free_reg, (node->data).n_val);
        return free_reg;
    }
    else if (node->node_type == NODE_TYPE_ID) {
        reg_index_t free_reg = get_free_register();
        load_addr_to_register(free_reg, get_addr(node->data.var_name), target_file);
        return free_reg;

    }

    char op = (node->data).c_val;
    reg_index_t l_val_reg = generate_arithmetic_code(node->left, target_file);
    reg_index_t r_val_reg = generate_arithmetic_code(node->right, target_file);
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


void generate_assignment_code(node_t* node, FILE* target_file) {
    if (!node || node->node_type != NODE_TYPE_ASSIGN || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_assignment_code} Something went wrong");
        exit(1);
    }

    node_t* id_node = node->left;
    int addr = get_addr(id_node->data.c_val);
    reg_index_t expr_output = generate_arithmetic_code(node->right, target_file);
    store_register_to_addr(expr_output, addr, target_file);
    free_register(expr_output);
}

reg_index_t generate_print_code(node_t* node, FILE* target_file) {
    if (!node || (node->node_type != NODE_TYPE_WRITE) || !(node->left)) {
        perror("{code_generation:generate_print_code} Something went wrong");
        exit(1);
    }

    reg_index_t ret_val;
    if (node->left->node_type == NODE_TYPE_ID) {
        node_t* id_node = node->left;
        int addr = get_addr(id_node->data.c_val);
        ret_val = print_addr(addr, target_file);
    }
    else if (node->left->node_type == NODE_TYPE_OPERATOR) {
        reg_index_t expr_output = generate_arithmetic_code(node->left, target_file);
        ret_val = print_register(expr_output, target_file);
        free_register(expr_output);
    }

    return ret_val;
}

reg_index_t generate_read_code(node_t* node, FILE* target_file) {
    if (!node || (node->node_type != NODE_TYPE_READ) || !(node->left) || (node->left->node_type != NODE_TYPE_ID)) {
        perror("{code_generation:generate_read_code} Something went wrong");
        exit(1);
    }

    reg_index_t ret_val;
    node_t* id_node = node->left;
    int addr = get_addr(id_node->data.c_val);
    ret_val = read_into_addr(addr, target_file);
    return ret_val;
}


void generate_statement_structure(node_t* node, FILE* target_file) {
    if (!node)
        return;

    if (node->node_type == NODE_TYPE_CONNECTOR) {
        generate_statement_structure(node->left, target_file);
        generate_statement_structure(node->right, target_file);
    }
    else if (node->node_type == NODE_TYPE_ASSIGN) {
        generate_assignment_code(node, target_file);
    } 
    else if (node->node_type == NODE_TYPE_WRITE) {
        reg_index_t ret_val = generate_print_code(node, target_file);
        free_register(ret_val);
    }
    else if (node->node_type == NODE_TYPE_READ) {
        reg_index_t ret_val = generate_read_code(node, target_file);
        free_register(ret_val);
    }
    else if (node->node_type == NODE_TYPE_OPERATOR || node->node_type == NODE_TYPE_VALUE || node->node_type == NODE_TYPE_ID)
        return;
}



void reset_registers() {
    for (reg_index_t i = 0; i < NUM_REGISTERS; i++)
        free_registers[i] = true;
}


reg_index_t print_register(reg_index_t data, FILE* target_file) {
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
    return ret_val;
}

reg_index_t print_addr(int addr, FILE* target_file) {
    fprintf(target_file, "\n\n");
    reg_index_t free_reg = get_free_register();
    
    load_addr_to_register(free_reg, addr, target_file);
    reg_index_t ret_val = print_register(free_reg, target_file);
    free_register(free_reg);

    return ret_val;
}


void load_addr_to_register(reg_index_t reg, int addr, FILE* target_file) {
    fprintf(target_file, "MOV R%d, [%d]\n", reg, addr);
}

void store_register_to_addr(reg_index_t reg, int addr, FILE* target_file) {
    fprintf(target_file, "MOV [%d], R%d\n", addr, reg);
}


reg_index_t read_into_addr(int addr, FILE* target_file) {
    fprintf(target_file, "\n\n");
    reg_index_t free_reg = get_free_register();

    fprintf(target_file, "MOV R%d, \"Read\"\nPUSH R%d\n", free_reg, free_reg);
    fprintf(target_file, "MOV R%d, -1\nPUSH R%d\n", free_reg, free_reg);
    fprintf(target_file, "MOV R%d, %d\nPUSH R%d\n", free_reg, addr, free_reg);
    fprintf(target_file, "PUSH R%d\n", free_reg);
    fprintf(target_file, "PUSH R%d\n", free_reg);
    fprintf(target_file, "CALL 0\n");

    reg_index_t ret_val = get_free_register();
    fprintf(target_file, "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n", ret_val, free_reg, free_reg, free_reg, free_reg);

    free_register(free_reg);
    fprintf(target_file, "\n\n");
    return ret_val;
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