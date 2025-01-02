#include "code_generation.h"

symbol_type_t is_type_compatible(symbol_type_t type1, symbol_type_t type2) {
    if (type1 == type2)
        return type1;
    else if (type1 == SYMBOL_TYPE_INT && type2 == SYMBOL_TYPE_PTR)
        return type1;
    else if (type1 == SYMBOL_TYPE_PTR && type2 == SYMBOL_TYPE_INT)
        return type2;
    else if (type1 == SYMBOL_TYPE_NOT_SET || type2 == SYMBOL_TYPE_NOT_SET)
        return SYMBOL_TYPE_NOT_SET;
    return -1;
}

int get_addr(symbol_table_t* var_entry) {
    return var_entry->binding;
}

reg_index_t generate_expression_code(node_t* node, FILE* target_file) {
    if (!node) {
        perror("{code_generation:generate_expression_code} Something weong wrong");
        exit(1);
    }

    if (node->node_type == NODE_TYPE_PTR_DEREF) {
        return generate_ptr_deref_code(node, target_file);
    }
    else if (node->node_type == NODE_TYPE_PTR_REF) {
        return generate_ptr_ref_code(node, target_file);
    }
    else if (node->node_type == NODE_TYPE_ARR_INDEX) {
        return generate_arr_index_code(node, target_file);
    }


    if (node->value_type == SYMBOL_TYPE_INT || node->value_type == SYMBOL_TYPE_PTR) {
        return generate_arithmetic_code(node, target_file);
    }
    else if (node->value_type == SYMBOL_TYPE_BOOL) {
        return generate_boolean_code(node, target_file);
    }
    else if (node->value_type == SYMBOL_TYPE_STR) {
        return generate_string_code(node, target_file);
    }
    else if (node->value_type == SYMBOL_TYPE_NOT_SET) {
        node_t* l_val = node->left;
        node_t* r_val = node->right;
        if (!l_val || !r_val) {
            perror("{code_generation:generate_expression_code} something went wrong");
            exit(1);
        }
        return generate_arithmetic_code(node, target_file);
    }
}

reg_index_t generate_string_code(node_t* node, FILE* target_file) {
    if (!node) {
        perror("{code_generation:generate_string_code} Something went wrong");
        exit(1);
    }

    reg_index_t free_reg = get_free_register();
    immediate_addressing_str(free_reg, node->data.s_val, target_file);
    return free_reg;
}

reg_index_t generate_boolean_code(node_t* node, FILE* target_file) {
    if (!node) {
        perror("{code_generation:generate_boolean_code} Something went wrong");
        exit(1);
    }

    if (node->node_type == NODE_TYPE_INT) {
        reg_index_t free_reg = get_free_register();

        immediate_addressing_int(free_reg, node->data.n_val, target_file);
        return free_reg;
    }
    else if (node->node_type == NODE_TYPE_ID) {
        reg_index_t free_reg = get_free_register();
        int address = get_addr(node->data.var_entry);
        direct_addressing_load(address, free_reg, target_file);
        return free_reg;

    }

    char relop[2];
    strcpy(relop, node->data.s_val);


    reg_index_t l_val_reg = generate_expression_code(node->left, target_file);
    reg_index_t r_val_reg = generate_expression_code(node->right, target_file);

    if (strcmp(relop, ">") == 0) {
        greater_than(l_val_reg, r_val_reg, target_file);
        free_register(r_val_reg);
        return l_val_reg;
    }
    else if (strcmp(relop, ">=") == 0) {
        greater_than_or_equals(l_val_reg, r_val_reg, target_file);
        free_register(r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "<") == 0) {
        less_than(l_val_reg, r_val_reg, target_file);
        free_register(r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "<=") == 0) {
        less_than_or_equals(l_val_reg, r_val_reg, target_file);
        free_register(r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "==") == 0) {
        equals(l_val_reg, r_val_reg, target_file);
        free_register(r_val_reg);
        return l_val_reg;  
    } 
    else if (strcmp(relop, "!=") == 0) {
        not_equals(l_val_reg, r_val_reg, target_file);
        free_register(r_val_reg);
        return l_val_reg;
    }  

    return -1;
}


reg_index_t generate_arr_index_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_arr_index_code}: Something went wrong.");
        exit(1);
    }

    node_t* id_node = node->left;
    if ((id_node->node_type != NODE_TYPE_ID) || (id_node->value_type != SYMBOL_TYPE_ARR)) {
        perror("{code_generation:generate_arr_index_code}: indexing can only be done on arrays");
        exit(1);
    }

    node_t* index_node = node->right;
    reg_index_t expr_output = generate_expression_code(index_node, target_file);
    if (index_node->value_type != SYMBOL_TYPE_INT) {
        perror("{code_generation:generate_arr_index_code}: arr index should be an int value");
        exit(1);
    }

    node->value_type = id_node->data.var_entry->inner_type;
    int id_address = get_addr(id_node->data.var_entry);
    add_immediate(expr_output, id_address, target_file);
    register_indirect_addressing_load(expr_output, expr_output, target_file);
    return expr_output;
}

reg_index_t generate_arithmetic_code(node_t* node, FILE* target_file) {
    if (!node) {
        perror("{code_generation:generate_arithmetic_code} Something went wrong");
        exit(1);
    }

    if (node->node_type == NODE_TYPE_INT) {
        reg_index_t free_reg = get_free_register();

        immediate_addressing_int(free_reg, (node->data).n_val, target_file);
        return free_reg;
    }
    else if (node->node_type == NODE_TYPE_ID) {
        reg_index_t free_reg = get_free_register();
        int addr = get_addr(node->data.var_entry);
        direct_addressing_load(addr, free_reg, target_file);
        return free_reg;

    }

    char op = (node->data).c_val;
    reg_index_t l_val_reg = generate_expression_code(node->left, target_file);
    reg_index_t r_val_reg = generate_expression_code(node->right, target_file);
    symbol_type_t output_type = is_type_compatible(node->left->value_type, node->right->value_type);
    if (output_type == -1) {
        perror("{code_generation:generate_arithmetic_code} Mismatched types");
        exit(1);
    }
    node->value_type = output_type;
    switch (op) {
        case '+':
            add_registers(l_val_reg, r_val_reg, target_file);
            break;
        case '-':           
            sub_registers(l_val_reg, r_val_reg, target_file);
            break;
        case '*':
            mul_registers(l_val_reg, r_val_reg, target_file);
            break;
        case '/':
            div_registers(l_val_reg, r_val_reg, target_file);
            break;
        case '%':
            mod_registers(l_val_reg, r_val_reg, target_file);
            break;
        default: 
            perror("unknown operator");
            exit(1);
    }
    free_register(r_val_reg);
    return l_val_reg;
}

reg_index_t generate_ptr_ref_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left)) {
        perror("{code_generation:genreate_ptr_ref_code} Something went wrong");
        exit(1);
    }

    reg_index_t expr_output = generate_expression_code(node->left, target_file);
    if (node->left->value_type != SYMBOL_TYPE_PTR) {
        perror("{code_generation:generate_ptr_ref_code} unary '*' operator not permitted on non-ptr variables");
        exit(1);
    }
    node->value_type = node->left->data.var_entry->inner_type;
    register_indirect_addressing_load(expr_output, expr_output, target_file);
    return expr_output;
}

reg_index_t generate_ptr_deref_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left)) {
        perror("{code_generation:generate_ptr_deref_code} Something went wrong");
        exit(1);
    }

    node_t* id_node = node->left;
    if (id_node->node_type != NODE_TYPE_ID) {
        perror("{code_generation:generate_ptr_deref_code} Dereference can only be done on variables");
        exit(1);
    }

    int address = get_addr(id_node->data.var_entry);
    reg_index_t free_reg = get_free_register();
    node->value_type = id_node->value_type;
    immediate_addressing_int(free_reg, address, target_file);
    return free_reg;
}


void generate_assignment_code(node_t* node, FILE* target_file) {
    if (!node || node->node_type != NODE_TYPE_ASSIGN || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_assignment_code} Something went wrong");
        exit(1);
    }


    if (node->left->node_type == NODE_TYPE_ARR_INDEX) {
        return generate_arr_assignment_code(node, target_file);
    }

    reg_index_t expr_output = generate_expression_code(node->right, target_file);
    symbol_type_t output_type = is_type_compatible(node->left->value_type, node->right->value_type);
    if (output_type == -1) {
        perror("{code_generation:generate_assignment_code} Type Mismatch");
        exit(1);
    }


    node_t* id_node = node->left;
    int addr = get_addr(id_node->data.var_entry);
    direct_addressing_store(addr, expr_output, target_file);
    free_register(expr_output);
}

void generate_arr_assignment_code(node_t* node, FILE* target_file) {
    node_t* arr_index_node = node->left;
    if (!arr_index_node || !(arr_index_node->left) || !(arr_index_node->right)) {
        perror("{code_generation:generate_arr_assignment_code} Something went wrong");
        exit(1);
    }

    node_t* id_node = arr_index_node->left;
        if ((id_node->node_type != NODE_TYPE_ID) || (id_node->value_type != SYMBOL_TYPE_ARR)) {
        perror("{code_generation:generate_arr_assignment_code}: indexing can only be done on arrays");
        exit(1);
    }

    node_t* index_node = arr_index_node->right;
    reg_index_t index_output = generate_expression_code(index_node, target_file);
    if (index_node->value_type != SYMBOL_TYPE_INT) {
        perror("{code_generation:generate_arr_assignment_code}: arr index should be an int value");
        exit(1);
    }

    arr_index_node->value_type = id_node->data.var_entry->inner_type;
    int id_address = get_addr(id_node->data.var_entry);
    add_immediate(index_output, id_address, target_file);
    reg_index_t expr_output = generate_expression_code(node->right, target_file);
    if (node->left->value_type != node->right->value_type) {
        perror("{code_generation:generate_arr_assignment_code} Type mismatch");
        exit(1);
    }

    register_indirect_addressing_store(index_output, expr_output, target_file);
    free_register(index_output);
    free_register(expr_output);

    return;
}

void generate_ptr_assignment_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_ptr_assignment_code} Something went wrong");
        exit(1);
    }

    node_t* id_node = node->left;
    if (id_node->value_type != SYMBOL_TYPE_PTR) {
        perror("{code_generation:generate_ptr_assignment_codee} Can't use '*' operator on a non-pointer variable");
        exit(1);
    }

    node_t* expr_node = node->right;
    reg_index_t expr_output = generate_expression_code(expr_node, target_file);
    if (expr_node->value_type != id_node->data.var_entry->inner_type) {
        perror("{code_generation:generate_ptr_assignment_code} Type mismatch");
        exit(1);
    }

    reg_index_t free_reg = get_free_register();
    int address = get_addr(id_node->data.var_entry);
    direct_addressing_load(address, free_reg, target_file);
    register_indirect_addressing_store(free_reg, expr_output, target_file);
    free_register(expr_output);
    free_register(free_reg);
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
    if (!node || (node->node_type != NODE_TYPE_READ) || !(node->left)) {
        perror("{code_generation:generate_read_code} Something went wrong");
        exit(1);
    }

    reg_index_t ret_val;
    if (node->left->node_type == NODE_TYPE_ID) {
        node_t* id_node = node->left;
        int addr = get_addr(id_node->data.var_entry);
        ret_val = read_into_addr(addr, target_file);
        return ret_val;
    }
    else if (node->left->node_type == NODE_TYPE_ARR_INDEX) {
        node_t* arr_index_node = node->left;
        node_t* id_node = arr_index_node->left;
        node_t* index_node = arr_index_node->right;

        if (!id_node || !index_node) {
            perror("{code_generation:generate_read_code} Something went wrong");
            exit(1);
        }

        reg_index_t free_reg = generate_expression_code(index_node, target_file);
        int addr = get_addr(id_node->data.var_entry);
        add_immediate(free_reg, addr, target_file);
        ret_val = read_into_reg_addr(free_reg, target_file);
        free_register(free_reg);
        return ret_val;
    }
}

void generate_if_code(node_t* node, FILE* target_file, label_index_t* break_label, label_index_t* continue_label) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_if_code} Something went wrong");
        exit(1);
    }
    node_t* cond_node = node->left;
    node_t* body_node = node->right;

    if (cond_node->value_type != SYMBOL_TYPE_BOOL) {
        perror("{node:generate_if_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t exit_if_label = get_label();

    reg_index_t condition_output = generate_expression_code(cond_node, target_file);
    jump_zero_to_label(condition_output, exit_if_label, target_file);
    generate_statement_structure(body_node, target_file, break_label, continue_label);
    add_label(exit_if_label, target_file);
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

    if (cond_node->value_type != SYMBOL_TYPE_BOOL) {
        perror("{node:generate_ifelse_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t else_body_label = get_label();
    label_index_t exit_if_label = get_label();

    reg_index_t condition_output = generate_expression_code(cond_node, target_file);
    jump_zero_to_label(condition_output, else_body_label, target_file);
    generate_statement_structure(if_body_node, target_file, break_label, continue_label);
    jump_to_label(exit_if_label, target_file);
    add_label(else_body_label, target_file);
    generate_statement_structure(else_body_node, target_file, break_label, continue_label);
    add_label(exit_if_label, target_file);
    free_register(condition_output);
}

void generate_while_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_while_code} Something went wrong");
        exit(1);
    }
    node_t* cond_node = node->left;
    node_t* body_node = node->right;

    if (cond_node->value_type != SYMBOL_TYPE_BOOL) {
        perror("{node:generate_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t condn_label = get_label();
    label_index_t exit_while_label = get_label();

    add_label(condn_label, target_file);
    reg_index_t condition_output = generate_expression_code(cond_node, target_file);
    jump_zero_to_label(condition_output, exit_while_label, target_file);
    generate_statement_structure(body_node, target_file, &exit_while_label, &condn_label);
    jump_to_label(condn_label, target_file);
    add_label(exit_while_label, target_file);
    free_register(condition_output);
} 

void generate_do_while_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_do_while_code} Something went wrong");
        exit(1);
    }

    node_t* cond_node = node->left;
    node_t* body_node = node->right;

    if (cond_node->value_type != SYMBOL_TYPE_BOOL) {
        perror("{node:generate_do_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t start_label = get_label();
    label_index_t condn_label = get_label();
    label_index_t end_do_while_label = get_label();
    add_label(start_label, target_file);
    generate_statement_structure(node->right, target_file, &end_do_while_label, &condn_label);
    add_label(condn_label, target_file);
    reg_index_t expr_output = generate_expression_code(node->left, target_file);
    jump_not_zero_to_label(expr_output, start_label, target_file);
    add_label(end_do_while_label, target_file);
    free_register(expr_output);
}

void generate_repeat_code(node_t* node, FILE* target_file) {
    if (!node || !(node->left) || !(node->right)) {
        perror("{code_generation:generate_do_while_code} Something went wrong");
        exit(1);
    }

    node_t* cond_node = node->left;
    node_t* body_node = node->right;

    if (cond_node->value_type != SYMBOL_TYPE_BOOL) {
        perror("{node:generate_do_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t start_label = get_label();
    label_index_t condn_label = get_label();
    label_index_t end_repeat_label = get_label();
    
    add_label(start_label, target_file);
    generate_statement_structure(node->right, target_file, &end_repeat_label, &condn_label);
    add_label(condn_label, target_file);
    reg_index_t expr_output = generate_expression_code(node->left, target_file);
    jump_zero_to_label(expr_output, start_label, target_file);
    add_label(end_repeat_label, target_file);
    free_register(expr_output);
}



void generate_statement_structure(node_t* node, FILE* target_file, label_index_t* break_label, label_index_t* continue_label) {
    if (!node)
        return;
    if (break_label && node->node_type == NODE_TYPE_BREAK) {
        jump_to_label(*break_label, target_file);
    }
    else if (continue_label && node->node_type == NODE_TYPE_CONTINUE) {
        jump_to_label(*continue_label, target_file);
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
    else if (node->node_type == NODE_TYPE_PTR_REF) {
        generate_ptr_assignment_code(node, target_file);
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


reg_index_t print_register(reg_index_t data, FILE* target_file) {
    reg_index_t func_name = get_free_register();
    reg_index_t arg1 = get_free_register();

    immediate_addressing_str(func_name, "\"Write\"", target_file);
    immediate_addressing_int(arg1, -2, target_file);

    call_library_function(func_name, arg1, data, data, data, target_file);

    free_register(func_name);
    free_register(arg1);

    reg_index_t ret_val = get_free_register();
    post_library_call(ret_val, data, target_file);

    return ret_val;
}

reg_index_t print_addr(int addr, FILE* target_file) {
    reg_index_t free_reg = get_free_register();
    
    direct_addressing_load(addr, free_reg, target_file);
    reg_index_t ret_val = print_register(free_reg, target_file);
    free_register(free_reg);

    return ret_val;
}

reg_index_t read_into_addr(int addr, FILE* target_file) {
    reg_index_t func_name = get_free_register();
    reg_index_t arg1 = get_free_register();
    reg_index_t arg2 = get_free_register();

    immediate_addressing_str(func_name, "\"Read\"", target_file);
    immediate_addressing_int(arg1, -1, target_file);
    immediate_addressing_int(arg2, addr, target_file);

    call_library_function(func_name, arg1, arg2, arg2, arg2, target_file);

    free_register(func_name);
    free_register(arg1);

    reg_index_t ret_val = get_free_register();
    post_library_call(ret_val, arg2, target_file);
    free_register(arg2);

    return ret_val;
}

reg_index_t read_into_reg_addr(reg_index_t reg_index, FILE* target_file) {
    reg_index_t func_name = get_free_register();
    reg_index_t arg1 = get_free_register();

    immediate_addressing_str(func_name, "\"Read\"", target_file);
    immediate_addressing_int(arg1, -1, target_file);

    call_library_function(func_name, arg1, reg_index, reg_index, reg_index, target_file);

    free_register(func_name);
    free_register(arg1);

    reg_index_t ret_val = get_free_register();
    post_library_call(ret_val, reg_index, target_file);
    free_register(reg_index);

    return ret_val;
}


void exit_program(FILE* target_file) {
    reg_index_t func_name = get_free_register();
    immediate_addressing_str(func_name, "\"Exit\"", target_file);
    reg_index_t ret_val = get_free_register();
    call_library_function(func_name, ret_val, ret_val, ret_val, ret_val, target_file);

    free_register(func_name);
    post_library_call(ret_val, ret_val, target_file);
}

void generate_headers(FILE* target_file) {
    fprintf(target_file, "0\n2056\n0\n0\n0\n0\n0\n0\n");
}


void generate_program(node_t* body, FILE* target_file) {
    reset_registers();
    reset_labels();

    generate_headers(target_file);
    add_breakpoint(target_file);
    int free_address = get_binding(0);
    fprintf(target_file, "MOV SP, %d\n", free_address);
    generate_statement_structure(body, target_file, NULL, NULL);
    exit_program(target_file);
}