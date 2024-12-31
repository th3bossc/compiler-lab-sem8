#include "code_generation.h"

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

label_index_t get_label() {
    used_labels++;
    return used_labels;
}

void reset_labels() {
    used_labels = 0;
}



void free_register(reg_index_t reg) {
    free_registers[reg] = true;
}

int get_addr(symbol_table_t* var_entry) {
    return var_entry->binding;
}

reg_index_t generate_expression_code(node_t* node, FILE* target_file) {
    if (!node) {
        perror("{code_generation:generate_expression_code} Something weong wrong");
        exit(1);
    }
    if (node->value_type == NODE_VALUE_INT) {
        return generate_arithmetic_code(node, target_file);
    }
    else if (node->value_type == NODE_VALUE_BOOL) {
        return generate_boolean_code(node, target_file);
    }
    else if (node->value_type == NODE_VALUE_STR) {
        return generate_string_code(node, target_file);
    }
    else if (node->value_type == NODE_VALUE_NOT_SET) {
        node_t* l_val = node->left;
        node_t* r_val = node->right;
        if (!l_val || !r_val) {
            perror("{code_generation:generate_expression_code} something went wrong");
            exit(1);
        }

        if (l_val->value_type != r_val->value_type) {
            perror("{code_generation.generate_expression_code}: mismatched types");
            exit(1);
        }

        if (l_val->value_type == NODE_VALUE_INT) {
            node->value_type = NODE_VALUE_INT;
            return generate_arithmetic_code(node, target_file);
        }
        else if (l_val->value_type == NODE_VALUE_BOOL) {
            node->value_type = NODE_VALUE_BOOL;
            return generate_boolean_code(node, target_file);
        }
    }
}

reg_index_t generate_string_code(node_t* node, FILE* target_file) {
    if (!node) {
        perror("{code_generation:generate_string_code} Something went wrong");
        exit(1);
    }

    reg_index_t free_reg = get_free_register();
    fprintf(target_file, "MOV R%d, %s\n", free_reg, node->data.s_val);
    return free_reg;
}

reg_index_t generate_boolean_code(node_t* node, FILE* target_file) {
    if (!node) {
        perror("{code_generation:generate_boolean_code} Something went wrong");
        exit(1);
    }

    if (node->node_type == NODE_TYPE_INT) {
        reg_index_t free_reg = get_free_register();

        fprintf(target_file, "MOV R%d, %d\n", free_reg, (node->data).n_val);
        return free_reg;
    }
    else if (node->node_type == NODE_TYPE_ID) {
        reg_index_t free_reg = get_free_register();
        load_addr_to_register(free_reg, get_addr(node->data.var_entry), target_file);
        return free_reg;

    }

    char relop[2];
    strcpy(relop, node->data.s_val);


    reg_index_t l_val_reg = generate_expression_code(node->left, target_file);
    reg_index_t r_val_reg = generate_expression_code(node->right, target_file);

    if (strcmp(relop, ">") == 0) {
        fprintf(target_file, "GT R%d, R%d\n", l_val_reg, r_val_reg);
        free_register(r_val_reg);
        return l_val_reg;
    }
    else if (strcmp(relop, ">=") == 0) {
        fprintf(target_file, "GE R%d, R%d\n", l_val_reg, r_val_reg);
        free_register(r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "<") == 0) {
        fprintf(target_file, "LT R%d, R%d\n", l_val_reg, r_val_reg);
        free_register(r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "<=") == 0) {
        fprintf(target_file, "LE R%d, R%d\n", l_val_reg, r_val_reg);
        free_register(r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "==") == 0) {      
        fprintf(target_file, "EQ R%d, R%d\n", l_val_reg, r_val_reg);
        free_register(r_val_reg);
        return l_val_reg;  
    } 
    else if (strcmp(relop, "!=") == 0) {
        fprintf(target_file, "NE R%d, R%d\n", l_val_reg, r_val_reg);
        free_register(r_val_reg);
        return l_val_reg;
    }  

    return -1;
}

reg_index_t generate_arithmetic_code(node_t* node, FILE* target_file) {

    if (!node) {
        perror("{code_generation:generate_arithmetic_code} Something went wrong");
        exit(1);
    }

    if (node->node_type == NODE_TYPE_INT) {
        reg_index_t free_reg = get_free_register();

        fprintf(target_file, "MOV R%d, %d\n", free_reg, (node->data).n_val);
        return free_reg;
    }
    else if (node->node_type == NODE_TYPE_ID) {
        reg_index_t free_reg = get_free_register();
        load_addr_to_register(free_reg, get_addr(node->data.var_entry), target_file);
        return free_reg;

    }

    char op = (node->data).c_val;
    reg_index_t l_val_reg = generate_expression_code(node->left, target_file);
    reg_index_t r_val_reg = generate_expression_code(node->right, target_file);
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

    reg_index_t expr_output = generate_expression_code(node->right, target_file);

    if (node->left->value_type != node->right->value_type) {
        perror("{code_generation:generate_assignment_code} Type Mismatch");
        exit(1);
    }


    node_t* id_node = node->left;
    int addr = get_addr(id_node->data.var_entry);
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
        int addr = get_addr(id_node->data.var_entry);
        ret_val = print_addr(addr, target_file);
    }
    else {
        reg_index_t expr_output = generate_expression_code(node->left, target_file);
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
    int addr = get_addr(id_node->data.var_entry);
    ret_val = read_into_addr(addr, target_file);
    return ret_val;
}

void generate_if_code(node_t* node, FILE* target_file, label_index_t* break_label, label_index_t* continue_label) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_if_code} Something went wrong");
        exit(1);
    }
    node_t* cond_node = node->left;
    node_t* body_node = node->right;

    if (cond_node->value_type != NODE_VALUE_BOOL) {
        perror("{node:generate_if_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t exit_if_label = get_label();

    reg_index_t condition_output = generate_expression_code(cond_node, target_file);
    fprintf(target_file, "JZ R%d, L%d\n", condition_output, exit_if_label);
    generate_statement_structure(body_node, target_file, break_label, continue_label);
    fprintf(target_file, "L%d:\n", exit_if_label);
    free_register(condition_output);
}

void generate_ifelse_code(node_t* node, FILE* target_file, label_index_t* break_label, label_index_t* continue_label) {
    if (!node || !(node->left) || !(node->right) || !(node->left->left) || !(node->left->right)) {
        perror("{code_generation:generate_ifelse_code} Something went wrong");
        exit(1);
    }
    node_t* cond_node = node->left->left;
    node_t* if_body_node = node->left->right;
    node_t* else_body_node = node->right;

    if (cond_node->value_type != NODE_VALUE_BOOL) {
        perror("{node:generate_ifelse_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t else_body_label = get_label();
    label_index_t exit_if_label = get_label();

    reg_index_t condition_output = generate_expression_code(cond_node, target_file);
    fprintf(target_file, "JZ R%d, L%d\n", condition_output, else_body_label);
    generate_statement_structure(if_body_node, target_file, break_label, continue_label);
    fprintf(target_file, "JMP L%d\n", exit_if_label);
    fprintf(target_file, "L%d:\n", else_body_label);
    generate_statement_structure(else_body_node, target_file, break_label, continue_label);
    fprintf(target_file, "L%d:\n", exit_if_label);
    free_register(condition_output);
}

void generate_while_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_while_code} Something went wrong");
        exit(1);
    }
    node_t* cond_node = node->left;
    node_t* body_node = node->right;

    if (cond_node->value_type != NODE_VALUE_BOOL) {
        perror("{node:generate_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t condn_label = get_label();
    label_index_t exit_while_label = get_label();

    fprintf(target_file, "L%d:\n", condn_label);
    reg_index_t condition_output = generate_expression_code(cond_node, target_file);
    fprintf(target_file, "JZ R%d, L%d\n", condition_output, exit_while_label);
    generate_statement_structure(body_node, target_file, &exit_while_label, &condn_label);
    fprintf(target_file, "JMP L%d\n", condn_label);
    fprintf(target_file, "L%d:\n", exit_while_label);
    free_register(condition_output);
} 

void generate_do_while_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_do_while_code} Something went wrong");
        exit(1);
    }

    node_t* cond_node = node->left;
    node_t* body_node = node->right;

    if (cond_node->value_type != NODE_VALUE_BOOL) {
        perror("{node:generate_do_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t start_label = get_label();
    label_index_t condn_label = get_label();
    label_index_t end_do_while_label = get_label();

    fprintf(target_file, "L%d:\n", start_label);
    generate_statement_structure(node->right, target_file, &end_do_while_label, &condn_label);
    fprintf(target_file, "L%d:\n", condn_label);
    reg_index_t expr_output = generate_expression_code(node->left, target_file);
    fprintf(target_file, "JNZ R%d, L%d\n", expr_output, start_label);
    fprintf(target_file, "L%d:\n", end_do_while_label);
    free_register(expr_output);
}

void generate_repeat_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_do_while_code} Something went wrong");
        exit(1);
    }

    node_t* cond_node = node->left;
    node_t* body_node = node->right;

    if (cond_node->value_type != NODE_VALUE_BOOL) {
        perror("{node:generate_do_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t start_label = get_label();
    label_index_t condn_label = get_label();
    label_index_t end_repeat_label = get_label();

    fprintf(target_file, "L%d:\n", start_label);
    generate_statement_structure(node->right, target_file, &end_repeat_label, &condn_label);
    fprintf(target_file, "L%d:\n", condn_label);
    reg_index_t expr_output = generate_expression_code(node->left, target_file);
    fprintf(target_file, "JZ R%d, L%d\n", expr_output, start_label);
    fprintf(target_file, "L%d:\n", end_repeat_label);
    free_register(expr_output);
}



void generate_statement_structure(node_t* node, FILE* target_file, label_index_t* break_label, label_index_t* continue_label) {
    if (!node)
        return;
    if (break_label && node->node_type == NODE_TYPE_BREAK) {
        fprintf(target_file, "JMP L%d\n", *break_label);
    }
    else if (continue_label && node->node_type == NODE_TYPE_CONTINUE) {
        fprintf(target_file, "JMP L%d\n", *continue_label);
    }
    if (node->node_type == NODE_TYPE_CONNECTOR) {
        generate_statement_structure(node->left, target_file, break_label, continue_label);
        generate_statement_structure(node->right, target_file, break_label, continue_label);
    }
    else if (node->node_type == NODE_TYPE_WRITE) {
        reg_index_t ret_val = generate_print_code(node, target_file);
        free_register(ret_val);
    }
    else if (node->node_type == NODE_TYPE_READ) {
        reg_index_t ret_val = generate_read_code(node, target_file);
        free_register(ret_val);
    }
    else if (node->node_type == NODE_TYPE_ASSIGN) {
        generate_assignment_code(node, target_file);
    } 
    else if (node->node_type == NODE_TYPE_IF) {
        generate_if_code(node, target_file, break_label, continue_label);
    }
    else if (node->node_type == NODE_TYPE_IFELSE) {
        generate_ifelse_code(node, target_file, break_label, continue_label);
    }
    else if (node->node_type == NODE_TYPE_WHILE) {
        generate_while_code(node, target_file);
    }
    else if (node->node_type == NODE_TYPE_DO_WHILE) {
        generate_do_while_code(node, target_file);
    }
    else if (node->node_type == NODE_TYPE_REPEAT) {
        generate_repeat_code(node, target_file);
    }
}


void reset_registers() {
    for (reg_index_t i = 0; i < NUM_REGISTERS; i++)
        free_registers[i] = true;
}



reg_index_t print_register(reg_index_t data, FILE* target_file) {
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
    return ret_val;
}

reg_index_t print_addr(int addr, FILE* target_file) {
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
    return ret_val;
}


void exit_program(FILE* target_file) {
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
}

void generate_headers(FILE* target_file) {
    fprintf(target_file, "0\n2056\n0\n0\n0\n0\n0\n0\n");
}


void generate_program(node_t* body, FILE* target_file) {
    reset_registers();
    reset_labels();

    generate_headers(target_file);
    fprintf(target_file, "BRKP\n");
    int free_address = get_binding(0);
    fprintf(target_file, "MOV SP, %d\n", free_address);
    generate_statement_structure(body, target_file, NULL, NULL);
    exit_program(target_file);
}