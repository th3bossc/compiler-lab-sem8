#include "code_generation.h"
// #include "../class_table/class_table.h"

type_table_t* is_type_compatible(type_table_t* type1, type_table_t* type2) {
    if (is_user_defined_type(type1) && (type2 == default_types->int_type)) 
        return type1;
    else if (is_user_defined_type(type2) && (type1 == default_types->int_type))
        return type2;
    if (type1 == type2)
        return type1;
    else if (type1 == default_types->int_type && type2 == default_types->ptr_type)
        return type2;
    else if (type1 == default_types->ptr_type && type2 == default_types->int_type)
        return type1;
    else if (type1 == default_types->unset_type || type2 == default_types->unset_type)
        return type1;
    return NULL;
}


reg_index_t load_var_data_to_reg(char* name, reg_index_t offset_reg) {
    activation_record_t* record = activation_stack_top();
    local_symbol_table_t* local_entry = local_symbol_table_lookup(record->l_symbol_table, name);
    if (local_entry == NULL) {
        global_symbol_table_t* global_entry = global_symbol_table_lookup(name);
        if (global_entry == NULL) {
            yyerror("{code_generation:load_var_data_to_reg} Variable not declared");
            exit(1);
        }
        else {
            reg_index_t free_reg = get_free_register(record->num_used_regs);
            int addr = global_entry->binding;
            immediate_addressing_int(free_reg, addr);
            if (offset_reg != -1)
                add_registers(free_reg, offset_reg);
            register_indirect_addressing_load(free_reg, free_reg);
            return free_reg;
        }
    }
    else {
        reg_index_t free_reg = get_free_register(record->num_used_regs);
        int relative_addr = local_entry->binding;
        fprintf(instr_set_fp, "MOV R%d, BP\n", free_reg);
        add_immediate(free_reg, relative_addr);
        register_indirect_addressing_load(free_reg, free_reg);
        return free_reg;
    }
}


reg_index_t load_var_addr_to_reg(char* name, reg_index_t offset_reg) {
    activation_record_t* record = activation_stack_top();
    local_symbol_table_t* local_entry = local_symbol_table_lookup(record->l_symbol_table, name);
    if (local_entry == NULL) {
        global_symbol_table_t* global_entry = global_symbol_table_lookup(name);
        if (global_entry == NULL) {
            yyerror("{code_generation:load_var_addr_to_reg} Variable not declared");
            exit(1);
        }
        else {
            reg_index_t free_reg = get_free_register(record->num_used_regs);
            int addr = global_entry->binding;
            immediate_addressing_int(free_reg, addr);
            if (offset_reg != -1)
                add_registers(free_reg, offset_reg);
            return free_reg;
        }
    }
    else {
        reg_index_t free_reg = get_free_register(record->num_used_regs);
        int relative_addr = local_entry->binding;
        fprintf(instr_set_fp, "MOV R%d, BP\n", free_reg);
        add_immediate(free_reg, relative_addr);
        return free_reg;
    }
}

reg_index_t load_class_addr_to_reg(global_symbol_table_t* global_entry, reg_index_t offset_reg) {
    activation_record_t* record = activation_stack_top();
    if (global_entry == NULL) {
        yyerror("{code_generation:load_class_addr_to_reg} Variable not declared");
        exit(1);
    }
    else {
        reg_index_t free_reg = get_free_register(record->num_used_regs);
        int addr = global_entry->binding;
        immediate_addressing_int(free_reg, addr);
        if (offset_reg != -1)
            add_registers(free_reg, offset_reg);
        return free_reg;
    }

}

reg_index_t generate_id_expr_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node) {
        yyerror("{code_generation:generate_id_expr_code} Something went wrong");
        exit(1);
    }

    type_table_t* id_type = get_var_type(node->data.s_val, record->l_symbol_table);
    if (is_tuple(id_type)) {
        node->value_type = id_type;
        reg_index_t id_val = load_var_addr_to_reg(node->data.s_val, -1);
        return id_val;
    }
    else {
        node->value_type = id_type;

        reg_index_t id_val = load_var_data_to_reg(node->data.s_val, -1);
        return id_val;
    }
}

reg_index_t generate_expression_code(ast_node_t* node) {
    if (!node) {
        yyerror("{code_generation:generate_expression_code} Something weong wrong");
        exit(1);
    }


    if (node->node_type == NODE_TYPE_ID) {
        return generate_id_expr_code(node);
    }
    if (node->node_type == NODE_TYPE_PTR_DEREF) {
        return generate_ptr_deref_code(node);
    }
    else if (node->node_type == NODE_TYPE_PTR_REF) {
        return generate_ptr_ref_code(node);
    }
    else if (node->node_type == NODE_TYPE_ARR_INDEX) {
        return generate_arr_index_code(node);
    }
    else if (node->node_type == NODE_TYPE_FUNC_CALL) {
        return generate_func_call_code(node);
    }
    else if (node->node_type == NODE_TYPE_TUPLE_FIELD) {
        return generate_tuple_index_code(node);
    }
    else if (node->node_type == NODE_TYPE_INIT_HEAP) {
        return generate_init_heap_code();
    }
    else if (node->node_type == NODE_TYPE_ALLOC) {
        return generate_alloc_code();
    }
    else if (node->node_type == NODE_TYPE_FREE) {
        return generate_free_heap_code(node);
    }
    else if (node->node_type == NODE_TYPE_SELF_FIELD) {
        return generate_self_field_code(node);
    }
    else if (node->node_type == NODE_TYPE_SELF_METHOD) {
        return generate_self_method_code(node);
    }

    if (node->value_type == default_types->int_type || node->value_type == default_types->ptr_type) {
        return generate_arithmetic_code(node);
    }
    else if (node->value_type == default_types->bool_type) {
        return generate_boolean_code(node);
    }
    else if (node->value_type == default_types->str_type) {
        return generate_string_code(node);
    }
    else if (node->value_type == default_types->unset_type) {
        ast_node_t* l_val = node->left;
        ast_node_t* r_val = node->right;
        if (!l_val || !r_val) {
            yyerror("{code_generation:generate_expression_code} something went wrong");
            exit(1);
        }
        return generate_arithmetic_code(node);
    }
}

reg_index_t generate_string_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node) {
        yyerror("{code_generation:generate_string_code} Something went wrong");
        exit(1);
    }

    reg_index_t free_reg = get_free_register(record->num_used_regs);
    immediate_addressing_str(free_reg, node->data.s_val);
    node->value_type = default_types->str_type;
    return free_reg;
}

reg_index_t generate_boolean_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node) {
        yyerror("{code_generation:generate_boolean_code} Something went wrong");
        exit(1);
    }

    if (node->node_type == NODE_TYPE_INT) {
        reg_index_t free_reg = get_free_register(record->num_used_regs);

        immediate_addressing_int(free_reg, node->data.n_val);
        return free_reg;
    }
    else if (node->node_type == NODE_TYPE_ID) {
        reg_index_t free_reg = load_var_data_to_reg(node->data.s_val, -1);
        return free_reg;
    }

    char* relop = strdup(node->data.s_val);


    if (strcmp(relop, "AND") == 0) {
        reg_index_t free_reg = get_free_register(record->num_used_regs);
        label_index_t exit_label = get_label();
        reg_index_t first_cond_output = generate_expression_code(node->left);
        register_addressing(free_reg, first_cond_output);
        jump_zero_to_label(free_reg, exit_label);
        reg_index_t second_cond_output = generate_expression_code(node->right);
        register_addressing(free_reg, second_cond_output);
        jump_not_zero_to_label(free_reg, exit_label);

        immediate_addressing_int(first_cond_output, 0);

        add_label(exit_label);
        free_used_register(record->num_used_regs, second_cond_output);
        free_used_register(record->num_used_regs, free_reg);
        return first_cond_output;
    }
    else if (strcmp(relop, "OR") == 0) {
        reg_index_t free_reg = get_free_register(record->num_used_regs);
        label_index_t exit_label = get_label();
        reg_index_t first_cond_output = generate_expression_code(node->left);
        register_addressing(free_reg, first_cond_output);
        jump_not_zero_to_label(free_reg, exit_label);
        reg_index_t second_cond_output = generate_expression_code(node->right);
        register_addressing(free_reg, second_cond_output);
        jump_zero_to_label(free_reg, exit_label);
        immediate_addressing_int(first_cond_output, 1);
        
        add_label(exit_label);
        free_used_register(record->num_used_regs, second_cond_output);
        free_used_register(record->num_used_regs, free_reg);
        return first_cond_output;
    }



    reg_index_t l_val_reg = generate_expression_code(node->left);
    reg_index_t r_val_reg = generate_expression_code(node->right);

    if (!is_type_compatible(node->left->value_type, node->right->value_type)) {
        yyerror("{generate_boolean_code} type mismatch");
        exit(1);
    }

    node->value_type = default_types->bool_type;

    if (strcmp(relop, ">") == 0) {
        greater_than(l_val_reg, r_val_reg);
        free_used_register(record->num_used_regs, r_val_reg);
        return l_val_reg;
    }
    else if (strcmp(relop, ">=") == 0) {
        greater_than_or_equals(l_val_reg, r_val_reg);
        free_used_register(record->num_used_regs, r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "<") == 0) {
        less_than(l_val_reg, r_val_reg);
        free_used_register(record->num_used_regs, r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "<=") == 0) {
        less_than_or_equals(l_val_reg, r_val_reg);
        free_used_register(record->num_used_regs, r_val_reg);
        return l_val_reg;
    } 
    else if (strcmp(relop, "==") == 0) {
        equals(l_val_reg, r_val_reg);
        free_used_register(record->num_used_regs, r_val_reg);
        return l_val_reg;  
    } 
    else if (strcmp(relop, "!=") == 0) {
        not_equals(l_val_reg, r_val_reg);
        free_used_register(record->num_used_regs, r_val_reg);
        return l_val_reg;
    }

    return -1;
}


reg_index_t generate_arr_index_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_arr_index_code}: Something went wrong.");
        exit(1);
    }

    ast_node_t* id_node = node->left;
    if ((id_node->node_type != NODE_TYPE_ID) || get_var_type(id_node->data.s_val, record->l_symbol_table) != default_types->arr_type) {
        yyerror("{code_generation:generate_arr_index_code}: indexing can only be done on arrays");
        exit(1);
    }

    ast_node_t* index_node = node->right;
    reg_index_t expr_output = generate_expression_code(index_node);
    if (index_node->value_type != default_types->int_type) {
        yyerror("{code_generation:generate_arr_index_code}: arr index should be an int value");
        exit(1);
    }

    node->value_type = get_var_inner_type(id_node->data.s_val, record->l_symbol_table);
    reg_index_t arr_data = load_var_data_to_reg(id_node->data.s_val, expr_output);
    free_used_register(record->num_used_regs, expr_output);
    return arr_data;
}

reg_index_t generate_arithmetic_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node) {
        yyerror("{code_generation:generate_arithmetic_code} Something went wrong");
        exit(1);
    }

    if (node->node_type == NODE_TYPE_INT) {
        reg_index_t free_reg = get_free_register(record->num_used_regs);

        immediate_addressing_int(free_reg, (node->data).n_val);
        return free_reg;
    }
    else if (node->node_type == NODE_TYPE_ID) {
        reg_index_t free_reg = load_var_data_to_reg(node->data.s_val, -1);
        return free_reg;

    }

    char op = (node->data).c_val;
    reg_index_t l_val_reg = generate_expression_code(node->left);
    reg_index_t r_val_reg = generate_expression_code(node->right);
    type_table_t* output_type = is_type_compatible(node->left->value_type, node->right->value_type);
    if (output_type == NULL) {
        yyerror("{code_generation:generate_arithmetic_code} Mismatched types");
        exit(1);
    }
    node->value_type = output_type;
    switch (op) {
        case '+':
            add_registers(l_val_reg, r_val_reg);
            break;
        case '-':           
            sub_registers(l_val_reg, r_val_reg);
            break;
        case '*':
            mul_registers(l_val_reg, r_val_reg);
            break;
        case '/':
            div_registers(l_val_reg, r_val_reg);
            break;
        case '%':
            mod_registers(l_val_reg, r_val_reg);
            break;
        default: 
            yyerror("unknown operator");
            exit(1);
    }
    free_used_register(record->num_used_regs, r_val_reg);
    return l_val_reg;
}

reg_index_t generate_func_call_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node) {
        yyerror("{code_generation:generate_func_call_code} Something went wrong");
        exit(1);
    }

    global_symbol_table_t* func_entry = global_symbol_table_lookup(node->data.s_val);
    if (func_entry->type != default_types->func_type) {
        yyerror("{code_generation:generate_func_call_code} Only functions can be called");
        exit(1);
    }

    node->value_type = func_entry->inner_type;
    label_index_t func_label = func_entry->binding;
    save_machine_state(record->num_used_regs);
    
    decl_node_t* it1 = func_entry->params;
    args_node_t* it2 = node->args_list;

    while(it1 != NULL && it2 != NULL) {
        reg_index_t arg_expr_output = generate_expression_code(it2->expr_node);
        if (it1->type != it2->expr_node->value_type) {
            print_prefix(it2->expr_node);
            yyerror("{code_generation:generate_func_call_code} Type mismatch in calling function");
            exit(1);
        }

        push_register(arg_expr_output);
        free_used_register(record->num_used_regs, arg_expr_output);

        it1 = it1->next;
        it2 = it2->next;
    }

    if (it1 != NULL || it2 != NULL) {
        yyerror("{code_generation:generate_func_call_code} Parameter count doesn't match");
        exit(1);
    }

    push_register(0);
    fprintf(instr_set_fp, "CALL L%d\n", func_label);
    pop_register(0);
    it1 = func_entry->params;
    reg_index_t free_reg = get_free_register(record->num_used_regs);
    while (it1 != NULL) {
        pop_register(free_reg);
        it1 = it1->next;
    }
    free_used_register(record->num_used_regs, free_reg);
    restore_machine_state(record->num_used_regs);

    reg_index_t ret_val = get_free_register(record->num_used_regs);
    register_addressing(ret_val, RESERVED_RETURN_REG);
    return ret_val;
}

reg_index_t generate_tuple_index_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_tuple_index_code} Something went wrong");
        exit(1);
    }

    reg_index_t id_addr = generate_expression_code(node->left);
    type_table_t* id_type = node->left->value_type;

    if (!is_user_defined_type(id_type) && !is_tuple(id_type)) {
        yyerror("{code_generation:generate_tuple_index_code} '.' operator can only be used on user defined types");
        exit(1);
    }

    ast_node_t* field_node = node->right;
    field_list_t* field_info = field_lookup(id_type, field_node->data.s_val);
    if (field_info == NULL) {
        yyerror("{code_generation:generate_tuple_index_code} field doesn't exist on type");
        exit(1);
    }

    field_node->value_type = field_info->type;
    node->value_type = field_info->type;
    reg_index_t field_index_reg = get_free_register(record->num_used_regs);
    immediate_addressing_int(field_index_reg, field_info->field_index);
    add_registers(id_addr, field_index_reg);
    reg_index_t id_data = get_free_register(record->num_used_regs);
    register_indirect_addressing_load(id_data, id_addr);
    free_used_register(record->num_used_regs, field_index_reg);
    free_used_register(record->num_used_regs, id_addr);
    return id_data;
}


reg_index_t generate_ptr_ref_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left)) {
        yyerror("{code_generation:genreate_ptr_ref_code} Something went wrong");
        exit(1);
    }


    reg_index_t expr_output = generate_expression_code(node->left);
    if (node->left->value_type != default_types->ptr_type) {
        yyerror("{code_generation:generate_ptr_ref_code} unary '*' operator not permitted on non-ptr variables");
        exit(1);
    }
    
    node->value_type = get_var_inner_type(node->left->data.s_val, record->l_symbol_table);
    if (is_primitive_type(node->value_type))
        register_indirect_addressing_load(expr_output, expr_output);
    return expr_output;
}

reg_index_t generate_ptr_deref_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left)) {
        yyerror("{code_generation:generate_ptr_deref_code} Something went wrong");
        exit(1);
    }

    ast_node_t* id_node = node->left;
    if (id_node->node_type != NODE_TYPE_ID) {
        yyerror("{code_generation:generate_ptr_deref_code} Dereference can only be done on variables");
        exit(1);
    }

    node->value_type = default_types->ptr_type;
    reg_index_t free_reg = load_var_addr_to_reg(id_node->data.s_val, -1);
    return free_reg;
}

reg_index_t generate_init_heap_code() {
    activation_record_t* record = activation_stack_top();
    return initialize_heap_code(record->num_used_regs);
}

reg_index_t generate_alloc_code() {
    activation_record_t* record = activation_stack_top();
    return alloc_memory(8);
}

reg_index_t generate_free_heap_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->middle)) {
        yyerror("{code_generation:generate_free_heap_code} Something went wrong");
        exit(1);
    }


    ast_node_t* id_node = node->middle;
    symbol_table_entry_t var_details = get_var_details(id_node->data.s_val, record->l_symbol_table);

    type_table_t* var_type;
    if (var_details.entry_type == ENTRY_NOT_FOUND) {
        yyerror("{code_generation:generate_free_heap_code} Variable not declared");
        exit(1);
    }

    if (var_details.entry_type == ENTRY_TYPE_GLOBAL)
        var_type = var_details.entry.global_entry->type;
    else 
        var_type = var_details.entry.local_entry->type;

    if (!is_user_defined_type(var_type)) {
        yyerror("{code_generation:generate_free_heap_code} Variable isn't a user defined type");
        exit(1);
    }

    reg_index_t addr_reg = load_var_data_to_reg(id_node->data.s_val, 0);
    reg_index_t ret_val = free_memory(addr_reg);
    free_used_register(record->num_used_regs, addr_reg);

    return ret_val;
}


reg_index_t generate_self_field_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    class_stack_t* current_object = class_stack_top();
    if (!node || !node->middle || current_object == NULL) {
        yyerror("{code_generation:generate_self_field_code} Something went wrong");
        exit(1);
    }

    if (record == NULL || (record->type != ACT_REC_TYPE_CLASS_METHOD)) {
        yyerror("{code_generation:generate_self_field_code} 'self' object can only be used within an instance of a class");
        exit(1);
    }


    ast_node_t* id_node = node->middle;
    class_table_t* class_details = current_object->class_var->type->class_details;

    class_field_t* field_details = class_field_lookup(class_details, id_node->data.s_val);
    if (field_details == NULL) {
        yyerror("{code_generation:generate_self_field_code} Field doesn't exit on class");
        exit(1);
    }
    reg_index_t offset_reg = get_free_register(record->num_used_regs);
    immediate_addressing_int(offset_reg, field_details->field_index);
    reg_index_t addr_reg = load_class_addr_to_reg(current_object->class_var, offset_reg);
    return addr_reg;
}


reg_index_t generate_self_method_code(ast_node_t* node) {
    
}


void generate_assignment_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || node->node_type != NODE_TYPE_ASSIGN || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_assignment_code} Something went wrong");
        exit(1);
    }


    if (node->left->node_type == NODE_TYPE_ARR_INDEX) {
        return generate_arr_assignment_code(node);
    }
    else if(node->left->node_type == NODE_TYPE_TUPLE_FIELD) {
        return generate_tuple_field_assignment_code(node);
    }
    ast_node_t* id_node = node->left;
    type_table_t* id_type = get_var_type(id_node->data.s_val, record->l_symbol_table);

    node->left->value_type = id_type;

    reg_index_t expr_output = generate_expression_code(node->right);
    type_table_t* output_type = is_type_compatible(node->left->value_type, node->right->value_type);
    node->value_type = default_types->void_type;
    if (output_type == NULL) {
        yyerror("{code_generation:generate_assignment_code} Type Mismatch");
        exit(1);
    }
    if (is_tuple(output_type)) {
        reg_index_t offset_reg = get_free_register(record->num_used_regs);
        int num_fields = output_type->num_fields;
        reg_index_t data_to_copy = get_free_register(record->num_used_regs);
        for (int i = 0; i < num_fields; i++) {
            immediate_addressing_int(offset_reg, i);
            reg_index_t free_reg = load_var_addr_to_reg(id_node->data.s_val, offset_reg);
            register_indirect_addressing_load(data_to_copy, expr_output);
            register_indirect_addressing_store(free_reg, data_to_copy);
            free_used_register(record->num_used_regs, free_reg);
            incr_register(expr_output);
        }
        free_used_register(record->num_used_regs, expr_output);
        free_used_register(record->num_used_regs, offset_reg);
        free_used_register(record->num_used_regs, data_to_copy);
    }
    else {
        ast_node_t* id_node = node->left;
        reg_index_t free_reg = load_var_addr_to_reg(id_node->data.s_val, -1);
        register_indirect_addressing_store(free_reg, expr_output);
        free_used_register(record->num_used_regs, free_reg);
        free_used_register(record->num_used_regs, expr_output);
    }
}

void generate_arr_assignment_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    ast_node_t* arr_index_node = node->left;
    if (!arr_index_node || !(arr_index_node->left) || !(arr_index_node->right)) {
        yyerror("{code_generation:generate_arr_assignment_code} Something went wrong");
        exit(1);
    }

    ast_node_t* id_node = arr_index_node->left;
    id_node->value_type = get_var_type(id_node->data.s_val, record->l_symbol_table);
    if ((id_node->node_type != NODE_TYPE_ID) || (id_node->value_type != default_types->arr_type)) {
        yyerror("{code_generation:generate_arr_assignment_code}: indexing can only be done on arrays");
        exit(1);
    }

    ast_node_t* index_node = arr_index_node->right;
    reg_index_t index_output = generate_expression_code(index_node);
    if (index_node->value_type != default_types->int_type) {
        yyerror("{code_generation:generate_arr_assignment_code}: arr index should be an int value");
        exit(1);
    }

    arr_index_node->value_type = get_var_inner_type(id_node->data.s_val, record->l_symbol_table);
    reg_index_t expr_output = generate_expression_code(node->right);
    reg_index_t arr_index_loc = load_var_addr_to_reg(id_node->data.s_val, index_output);
    
    type_table_t* output_type = is_type_compatible(node->left->value_type, node->right->value_type);
    if (output_type == NULL) {
        yyerror("{code_generation:generate_arr_assignment_code} Type mismatch");
        exit(1);
    }

    node->value_type = default_types->void_type;

    register_indirect_addressing_store(arr_index_loc, expr_output);
    free_used_register(record->num_used_regs, index_output);
    free_used_register(record->num_used_regs, expr_output);
    free_used_register(record->num_used_regs, arr_index_loc);

    return;
}

void generate_ptr_assignment_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_ptr_assignment_code} Something went wrong");
        exit(1);
    }

    ast_node_t* id_node = node->left;
    if (get_var_type(id_node->data.s_val, record->l_symbol_table) != default_types->ptr_type) {
        yyerror("{code_generation:generate_ptr_assignment_codee} Can't use '*' operator on a non-pointer variable");
        exit(1);
    }

    ast_node_t* expr_node = node->right;
    reg_index_t expr_output = generate_expression_code(expr_node);
    if (expr_node->value_type != get_var_inner_type(id_node->data.s_val, record->l_symbol_table)) {
        yyerror("{code_generation:generate_ptr_assignment_code} Type mismatch");
        exit(1);
    }

    node->value_type = default_types->void_type;

    reg_index_t ptr_data = load_var_data_to_reg(id_node->data.s_val, -1);
    register_indirect_addressing_store(ptr_data, expr_output);
    free_used_register(record->num_used_regs, expr_output);
    free_used_register(record->num_used_regs, ptr_data);
}

void generate_tuple_field_assignment_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_tuple_field_assignment_code} Something went wrong");
        exit(1);
    }

    ast_node_t* id_node = node->left;
    ast_node_t* expr_node = node->right;

    reg_index_t id_addr = generate_expression_code(id_node->left);
    type_table_t* id_type = id_node->left->value_type;
    if (!is_user_defined_type(id_type)) {
        yyerror("{code_generation:generate_tuple_field_assignment_code} '.' operator is only permitted on user defined types");
        exit(1);
    }

    ast_node_t* field_node = id_node->right;
    field_list_t* field_info = field_lookup(id_type, field_node->data.s_val);
    if (field_info == NULL) {
        yyerror("{code_generation:generate_tuple_field_assignment_code} field doesn't exist on type");
        exit(1);
    }
    field_node->value_type = field_info->type;
    id_node->value_type = id_type;
    reg_index_t field_index_reg = get_free_register(record->num_used_regs);
    immediate_addressing_int(field_index_reg, field_info->field_index);
    add_registers(id_addr, field_index_reg);
    free_used_register(record->num_used_regs, field_index_reg);
    reg_index_t expr_output = generate_expression_code(expr_node);

    type_table_t* output_type = is_type_compatible(expr_node->value_type, field_node->value_type);
    if (output_type == NULL) {
        yyerror("{code_generation:generate_tuple_field_assignment_code} type mismatch");
        exit(1);
    }

    node->value_type = default_types->void_type;

    register_indirect_addressing_store(id_addr, expr_output);
    free_used_register(record->num_used_regs, id_addr);
    free_used_register(record->num_used_regs, expr_output);
}

reg_index_t generate_print_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || (node->node_type != NODE_TYPE_WRITE) || !(node->left)) {
        yyerror("{code_generation:generate_print_code} Something went wrong");
        exit(1);
    }

    reg_index_t ret_val;
    if (node->left->node_type == NODE_TYPE_ID) {
        ast_node_t* id_node = node->left;
        reg_index_t addr_reg = load_var_data_to_reg(id_node->data.s_val, -1);
        ret_val = print_register(addr_reg);
        free_used_register(record->num_used_regs, addr_reg);
    }
    else {
        reg_index_t expr_output = generate_expression_code(node->left);
        ret_val = print_register(expr_output);
        free_used_register(record->num_used_regs, expr_output);
    }

    node->value_type = default_types->int_type;

    return ret_val;
}

reg_index_t generate_read_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || (node->node_type != NODE_TYPE_READ) || !(node->left)) {
        yyerror("{code_generation:generate_read_code} Something went wrong");
        exit(1);
    }

    reg_index_t ret_val;
    if (node->left->node_type == NODE_TYPE_ID) {
        ast_node_t* id_node = node->left;
        reg_index_t addr_reg = load_var_addr_to_reg(id_node->data.s_val, -1);
        ret_val = read_into_reg_addr(addr_reg);
        free_used_register(record->num_used_regs, addr_reg);
        
        node->value_type = default_types->int_type;
        return ret_val;
    }
    else if (node->left->node_type == NODE_TYPE_ARR_INDEX) {
        ast_node_t* arr_index_node = node->left;
        ast_node_t* id_node = arr_index_node->left;
        ast_node_t* index_node = arr_index_node->right;

        if (!id_node || !index_node) {
            yyerror("{code_generation:generate_read_code} Something went wrong");
            exit(1);
        }

        reg_index_t free_reg = generate_expression_code(index_node);
        reg_index_t addr_reg = load_var_addr_to_reg(id_node->data.s_val, free_reg);
        ret_val = read_into_reg_addr(addr_reg);
        free_used_register(record->num_used_regs, free_reg);
        free_used_register(record->num_used_regs, addr_reg);

        node->value_type = default_types->int_type;
        return ret_val;
    }
    else if (node->left->node_type == NODE_TYPE_TUPLE_FIELD) {
        ast_node_t* id_node = node->left;
        reg_index_t id_addr = generate_expression_code(id_node->left);
        type_table_t* id_type = id_node->left->value_type;
        if (!is_user_defined_type(id_type)) {
        yyerror("{code_generation:generate_read_code} '.' operator is only permitted on user defined types");
        exit(1);
        }

        ast_node_t* field_node = id_node->right;
        field_list_t* field_info = field_lookup(id_type, field_node->data.s_val);
        if (field_info == NULL) {
            yyerror("{code_generation:generate_tuple_field_assignment_code} field doesn't exist on type");
            exit(1);
        }

        field_node->value_type = field_info->type;
        id_node->value_type = field_info->type;
        reg_index_t field_index_reg = get_free_register(record->num_used_regs);
        immediate_addressing_int(field_index_reg, field_info->field_index);
        add_registers(id_addr, field_index_reg);
        free_used_register(record->num_used_regs, field_index_reg);
        ret_val = read_into_reg_addr(id_addr);
        free_used_register(record->num_used_regs, id_addr);

        node->value_type = default_types->int_type;
        return ret_val;
    }
}

void generate_if_code(ast_node_t* node, label_index_t* break_label, label_index_t* continue_label) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->middle)) {
        yyerror("{code_generation:generate_if_code} Something went wrong");
        exit(1);
    }
    ast_node_t* cond_node = node->middle;
    ast_node_t* body_node = node->left;

    if (cond_node->value_type != default_types->bool_type) {
        yyerror("{node:generate_if_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t exit_if_label = get_label();

    reg_index_t condition_output = generate_expression_code(cond_node);
    jump_zero_to_label(condition_output, exit_if_label);
    generate_statement_structure(body_node, break_label, continue_label);
    add_label(exit_if_label);
    free_used_register(record->num_used_regs, condition_output);

    node->value_type = default_types->void_type;
}

void generate_ifelse_code(ast_node_t* node, label_index_t* break_label, label_index_t* continue_label) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->right) || !(node->middle)) {
        yyerror("{code_generation:generate_ifelse_code} Something went wrong");
        exit(1);
    }
    ast_node_t* cond_node = node->middle;
    ast_node_t* if_body_node = node->left;
    ast_node_t* else_body_node = node->right;

    if (cond_node->value_type != default_types->bool_type) {
        yyerror("{node:generate_ifelse_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t else_body_label = get_label();
    label_index_t exit_if_label = get_label();

    reg_index_t condition_output = generate_expression_code(cond_node);
    jump_zero_to_label(condition_output, else_body_label);
    generate_statement_structure(if_body_node, break_label, continue_label);
    jump_to_label(exit_if_label);
    add_label(else_body_label);
    generate_statement_structure(else_body_node, break_label, continue_label);
    add_label(exit_if_label);
    free_used_register(record->num_used_regs, condition_output);

    node->value_type = default_types->void_type;
}

void generate_while_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_while_code} Something went wrong");
        exit(1);
    }
    ast_node_t* cond_node = node->left;
    ast_node_t* body_node = node->right;

    if (cond_node->value_type != default_types->bool_type) {
        yyerror("{node:generate_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t condn_label = get_label();
    label_index_t exit_while_label = get_label();
    add_label(condn_label);
    reg_index_t condition_output = generate_expression_code(cond_node);
    jump_zero_to_label(condition_output, exit_while_label);
    generate_statement_structure(body_node, &exit_while_label, &condn_label);
    jump_to_label(condn_label);
    add_label(exit_while_label);
    free_used_register(record->num_used_regs, condition_output);

    node->value_type = default_types->void_type;
} 

void generate_do_while_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_do_while_code} Something went wrong");
        exit(1);
    }

    ast_node_t* cond_node = node->left;
    ast_node_t* body_node = node->right;

    if (cond_node->value_type != default_types->bool_type) {
        yyerror("{node:generate_do_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t start_label = get_label();
    label_index_t condn_label = get_label();
    label_index_t end_do_while_label = get_label();
    add_label(start_label);
    generate_statement_structure(node->right, &end_do_while_label, &condn_label);
    add_label(condn_label);
    reg_index_t expr_output = generate_expression_code(node->left);
    jump_not_zero_to_label(expr_output, start_label);
    add_label(end_do_while_label);
    free_used_register(record->num_used_regs, expr_output);

    node->value_type = default_types->void_type;
}

void generate_repeat_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_do_while_code} Something went wrong");
        exit(1);
    }

    ast_node_t* cond_node = node->left;
    ast_node_t* body_node = node->right;

    if (cond_node->value_type != default_types->bool_type) {
        yyerror("{node:generate_do_while_code} IF condn doesn't return a boolean value");
        exit(1);
    }

    label_index_t start_label = get_label();
    label_index_t condn_label = get_label();
    label_index_t end_repeat_label = get_label();
    
    add_label(start_label);
    generate_statement_structure(node->right, &end_repeat_label, &condn_label);
    add_label(condn_label);
    reg_index_t expr_output = generate_expression_code(node->left);
    jump_zero_to_label(expr_output, start_label);
    add_label(end_repeat_label);
    free_used_register(record->num_used_regs, expr_output);

    node->value_type = default_types->void_type;
}

void generate_program_structure(ast_node_t* node) {
    if (!node) {
        return;
    }
    if (node->node_type == NODE_TYPE_FUNC_DECL) {
        generate_function_code(node);
    }
    else {
        generate_program_structure(node->left);
        generate_program_structure(node->right);
    }
}

void generate_function_code(ast_node_t* node) {
    if (!node || (node->node_type != NODE_TYPE_FUNC_DECL)) {
        yyerror("{code_generation:generate_function_code} Something went wrong");
        exit(1);
    }


    global_symbol_table_t* func_entry = node->func_details;
    label_index_t func_label = func_entry->binding;
    if (!func_entry) {
        yyerror("{code_generation:generate_function_code} Function decl not found");
        exit(1);
    }
    

    local_symbol_table_t* local_symbol_table = NULL;

    decl_node_t* func_params = func_entry->params;
    decl_node_t* func_local_decls = func_entry->local_decls;

    int i = 1;
    for (decl_node_t* entry = func_params; entry != NULL; entry = entry->next) {
        local_symbol_table_t* new_entry = create_local_symbol_table_entry(
            entry->name,
            entry->type,
            entry->inner_type,
            i,
            NULL
        );
        local_symbol_table = append_to_local_table(local_symbol_table, new_entry);
        i++;
    }

    int num_params = i-1;

    for (decl_node_t* entry = func_local_decls; entry != NULL; entry = entry->next) {
        local_symbol_table_t* new_entry = create_local_symbol_table_entry(
            entry->name,
            entry->type,
            entry->inner_type,
            i,
            NULL
        );

        local_symbol_table = append_to_local_table(local_symbol_table, new_entry);
        i++;



    }
    add_label(func_label);
    fprintf(instr_set_fp, "PUSH BP\n");
    fprintf(instr_set_fp, "MOV BP, SP\n");
    fprintf(instr_set_fp, "ADD SP, %d\n", i-1);


    int* num_used_registers = (int*) calloc(NUM_REGISTERS, sizeof(int));
    reset_registers(num_used_registers);
    

    //copy args to local table
    for (int i = 0; i < num_params; i++) {
        reg_index_t dest_reg = get_free_register(num_used_registers);
        reg_index_t src_reg = get_free_register(num_used_registers);

        add_breakpoint();
        fprintf(instr_set_fp, "MOV R%d, BP\n", src_reg);
        fprintf(instr_set_fp, "SUB R%d, %d\n", src_reg, (num_params + 2 - i));
        fprintf(instr_set_fp, "MOV R%d, [R%d]\n", src_reg, src_reg);

        fprintf(instr_set_fp, "MOV R%d, BP\n", dest_reg);
        fprintf(instr_set_fp, "ADD R%d, %d\n", dest_reg, i+1);

        fprintf(instr_set_fp, "MOV [R%d], R%d\n", dest_reg, src_reg);
    }

    activation_stack_push_func_entry(func_entry, local_symbol_table, num_used_registers);
    generate_statement_structure(node->middle, NULL, NULL);
    activation_stack_pop();
    
    destroy_local_symbol_table(local_symbol_table);
    free(num_used_registers);
}

void generate_class_method_code(class_method_t* method) {
    if (!method || !method->func_body) {
        yyerror("{code_generation:generate_class_method_code} Method declared but not initialized");
        exit(1);
    }
    label_index_t func_label = method->label;

    decl_node_t* func_params = method->params;
    decl_node_t* func_local_decls = method->local_decls;
    local_symbol_table_t* local_symbol_table = NULL;

        int i = 1;
    for (decl_node_t* entry = func_params; entry != NULL; entry = entry->next) {
        local_symbol_table_t* new_entry = create_local_symbol_table_entry(
            entry->name,
            entry->type,
            entry->inner_type,
            i,
            NULL
        );
        local_symbol_table = append_to_local_table(local_symbol_table, new_entry);
        i++;
    }

    int num_params = i-1;

    for (decl_node_t* entry = func_local_decls; entry != NULL; entry = entry->next) {
        local_symbol_table_t* new_entry = create_local_symbol_table_entry(
            entry->name,
            entry->type,
            entry->inner_type,
            i,
            NULL
        );

        local_symbol_table = append_to_local_table(local_symbol_table, new_entry);
        i++;



    }
    add_label(func_label);
    fprintf(instr_set_fp, "PUSH BP\n");
    fprintf(instr_set_fp, "MOV BP, SP\n");
    fprintf(instr_set_fp, "ADD SP, %d\n", i-1);


    int* num_used_registers = (int*) calloc(NUM_REGISTERS, sizeof(int));
    reset_registers(num_used_registers);
    

    //copy args to local table
    for (int i = 0; i < num_params; i++) {
        reg_index_t dest_reg = get_free_register(num_used_registers);
        reg_index_t src_reg = get_free_register(num_used_registers);

        add_breakpoint();
        fprintf(instr_set_fp, "MOV R%d, BP\n", src_reg);
        fprintf(instr_set_fp, "SUB R%d, %d\n", src_reg, (num_params + 2 - i));
        fprintf(instr_set_fp, "MOV R%d, [R%d]\n", src_reg, src_reg);

        fprintf(instr_set_fp, "MOV R%d, BP\n", dest_reg);
        fprintf(instr_set_fp, "ADD R%d, %d\n", dest_reg, i+1);

        fprintf(instr_set_fp, "MOV [R%d], R%d\n", dest_reg, src_reg);
    }

    activation_stack_push_method_entry(method, local_symbol_table, num_used_registers);
    generate_statement_structure(method->func_body, NULL, NULL);
    activation_stack_pop();
    
    destroy_local_symbol_table(local_symbol_table);
    free(num_used_registers);
}

void generate_class_code() {
    for (class_table_t* class_details = class_table; class_details != NULL; class_details = class_details->next) {
        for (class_method_t* method = class_details->methods; method != NULL; method = method->next) {
            generate_class_method_code(method);
        }
    }
}

void generate_func_return_code(ast_node_t* node) {
    activation_record_t* record = activation_stack_top();
    if (!node || record == NULL) {
        yyerror("{code_generation:generate_func_return_code} Something went wrong");
        exit(1);
    }

    type_table_t* return_type = (record->type == ACT_REC_TYPE_CLASS_METHOD) 
        ? record->entry.method_entry->return_type 
        : record->entry.func_entry->inner_type;

    if (node->middle == NULL) {
        if (return_type != default_types->void_type) {
            yyerror("{code_generation:generate_func_return_code} Mismatched return types");
            exit(1);
        }

        fprintf(instr_set_fp, "MOV SP, BP\n");
        fprintf(instr_set_fp, "POP BP\n");
        fprintf(instr_set_fp, "RET\n");
        return;
    }
    ast_node_t* expr_node = node->middle;
    reg_index_t expr_output_reg = generate_expression_code(expr_node);
    if (return_type != expr_node->value_type) {
        yyerror("{code_generation:generate_func_return_code} Mismatched return types");
        exit(1);
    }

    reg_index_t ret_val_reg = get_free_register(record->num_used_regs);
    fprintf(instr_set_fp, "MOV R%d, BP\n", ret_val_reg);
    sub_immediate(ret_val_reg, 2);
    register_indirect_addressing_store(ret_val_reg, expr_output_reg);
    free_used_register(record->num_used_regs, expr_output_reg);
    free_used_register(record->num_used_regs, ret_val_reg);

    node->value_type = expr_node->value_type;

    fprintf(instr_set_fp, "MOV SP, BP\n");
    fprintf(instr_set_fp, "POP BP\n");
    fprintf(instr_set_fp, "RET\n");
}


void generate_statement_structure( ast_node_t* node, label_index_t* break_label,  label_index_t* continue_label) {
    activation_record_t* record = activation_stack_top();
    if (!node)
        return;
    if (break_label && node->node_type == NODE_TYPE_BREAK) {
        jump_to_label(*break_label);
    }
    else if (continue_label && node->node_type == NODE_TYPE_CONTINUE) {
        jump_to_label(*continue_label);
    }
    if (node->node_type == NODE_TYPE_CONNECTOR) {
        generate_statement_structure(node->left, break_label, continue_label);
        generate_statement_structure(node->right, break_label, continue_label);
    }
    else if (node->node_type == NODE_TYPE_WRITE) {
        reg_index_t ret_val = generate_print_code(node);
        free_used_register(record->num_used_regs, ret_val);
    }
    else if (node->node_type == NODE_TYPE_READ) {
        reg_index_t ret_val = generate_read_code(node);
        free_used_register(record->num_used_regs, ret_val);
    }
    else if (node->node_type == NODE_TYPE_ASSIGN) {
        generate_assignment_code(node);
    } 
    else if (node->node_type == NODE_TYPE_PTR_REF) {
        generate_ptr_assignment_code(node);
    }
    else if (node->node_type == NODE_TYPE_IF) {
        generate_if_code(node, break_label, continue_label);
    }
    else if (node->node_type == NODE_TYPE_IFELSE) {
        generate_ifelse_code(node, break_label, continue_label);
    }
    else if (node->node_type == NODE_TYPE_WHILE) {
        generate_while_code(node);
    }
    else if (node->node_type == NODE_TYPE_DO_WHILE) {
        generate_do_while_code(node);
    }
    else if (node->node_type == NODE_TYPE_REPEAT) {
        generate_repeat_code(node);
    }
    else if (node->node_type == NODE_TYPE_FUNC_RET) {
        generate_func_return_code(node);
    }
    else if (node->node_type == NODE_TYPE_FUNC_CALL) {
        reg_index_t ret_val_reg = generate_func_call_code(node);
        free_used_register(record->num_used_regs, ret_val_reg);
    }
    else if (node->node_type == NODE_TYPE_INIT_HEAP) {
        reg_index_t ret_val = generate_init_heap_code();
        free_used_register(record->num_used_regs, ret_val);
    }
    else if (node->node_type == NODE_TYPE_FREE) {
        reg_index_t ret_val = generate_free_heap_code(node);
        free_used_register(record->num_used_regs, ret_val);
    }
}


reg_index_t print_register(reg_index_t data) {
    activation_record_t* record = activation_stack_top();
    reg_index_t func_name = get_free_register(record->num_used_regs);
    reg_index_t arg1 = get_free_register(record->num_used_regs);
    reg_index_t ret_val = RESERVED_RETURN_REG;
    immediate_addressing_str(func_name, "\"Write\"");
    immediate_addressing_int(arg1, -2);

    call_library_function(func_name, arg1, data, data, data, record->num_used_regs);
    post_library_call(ret_val, data, record->num_used_regs);

    free_used_register(record->num_used_regs, func_name);
    free_used_register(record->num_used_regs, arg1);
    return ret_val;
}

reg_index_t print_addr(int addr) {
    activation_record_t* record = activation_stack_top();
    reg_index_t free_reg = get_free_register(record->num_used_regs);
    
    direct_addressing_load(addr, free_reg);
    reg_index_t ret_val = print_register(free_reg);
    free_used_register(record->num_used_regs, free_reg);

    return ret_val;
}

reg_index_t read_into_addr(int addr) {
    activation_record_t* record = activation_stack_top();
    reg_index_t func_name = get_free_register(record->num_used_regs);
    reg_index_t arg1 = get_free_register(record->num_used_regs);
    reg_index_t arg2 = get_free_register(record->num_used_regs);
    reg_index_t ret_val = RESERVED_RETURN_REG;

    immediate_addressing_str(func_name, "\"Read\"");
    immediate_addressing_int(arg1, -1);
    immediate_addressing_int(arg2, addr);

    call_library_function(func_name, arg1, arg2, arg2, arg2, record->num_used_regs);
    post_library_call(ret_val, arg2, record->num_used_regs);

    free_used_register(record->num_used_regs, func_name);
    free_used_register(record->num_used_regs, arg1);
    free_used_register(record->num_used_regs, arg2);

    return ret_val;
}

reg_index_t read_into_reg_addr(reg_index_t reg_index) {
    activation_record_t* record = activation_stack_top();
    reg_index_t func_name = get_free_register(record->num_used_regs);
    reg_index_t arg1 = get_free_register(record->num_used_regs);
    reg_index_t ret_val = RESERVED_RETURN_REG;

    immediate_addressing_str(func_name, "\"Read\"");
    immediate_addressing_int(arg1, -1);

    call_library_function(func_name, arg1, reg_index, reg_index, reg_index, record->num_used_regs);
    post_library_call(ret_val, reg_index, record->num_used_regs);

    free_used_register(record->num_used_regs, func_name);
    free_used_register(record->num_used_regs, arg1);

    return ret_val;
}


reg_index_t initialize_heap_code() {
    activation_record_t* record = activation_stack_top();
    reg_index_t func_name = get_free_register(record->num_used_regs);
    reg_index_t ret_val = RESERVED_RETURN_REG;
    reg_index_t free_reg = get_free_register(record->num_used_regs);

    immediate_addressing_str(func_name, "\"Heapset\"");

    call_library_function(func_name, ret_val, ret_val, ret_val, ret_val, record->num_used_regs);
    post_library_call(ret_val, free_reg, record->num_used_regs);

    free_used_register(record->num_used_regs, func_name);
    free_used_register(record->num_used_regs, free_reg);

    return ret_val;
}

reg_index_t alloc_memory(int size) {
    activation_record_t* record = activation_stack_top();
    add_breakpoint();
    reg_index_t func_name = get_free_register(record->num_used_regs);
    reg_index_t ret_val = RESERVED_RETURN_REG;
    reg_index_t arg_val = get_free_register(record->num_used_regs);

    immediate_addressing_str(func_name, "\"Alloc\"");
    immediate_addressing_int(arg_val, size);

    call_library_function(func_name, arg_val, arg_val, arg_val, ret_val, record->num_used_regs);
    post_library_call(ret_val, arg_val, record->num_used_regs);

    free_used_register(record->num_used_regs, func_name);
    free_used_register(record->num_used_regs, arg_val);

    return ret_val;
}

reg_index_t free_memory(reg_index_t location) {
    activation_record_t* record = activation_stack_top();
    reg_index_t func_name = get_free_register(record->num_used_regs);
    reg_index_t ret_val = RESERVED_RETURN_REG;
    
    immediate_addressing_str(func_name, "\"Free\"");

    call_library_function(func_name, location, location, location, ret_val, record->num_used_regs);
    post_library_call(ret_val, func_name, record->num_used_regs);

    free_used_register(record->num_used_regs, func_name);

    return ret_val;
}



void exit_program(int* free_registers) {
    reg_index_t func_name = get_free_register(free_registers);
    reg_index_t ret_val = RESERVED_RETURN_REG;

    immediate_addressing_str(func_name, "\"Exit\"");
    
    call_library_function(func_name, ret_val, ret_val, ret_val, ret_val, free_registers);
    post_library_call(ret_val, ret_val, free_registers);

    free_used_register(free_registers, func_name);
    free_used_register(free_registers, ret_val);
}

void generate_headers() {
    fprintf(instr_set_fp, "0\n2056\n0\n0\n0\n0\n0\n0\n");
}


void generate_program(ast_node_t* body) {
    class_stack_initialize();
    activation_stack_initialize();
    generate_headers();
    add_breakpoint();
    int free_address = get_binding(0);
    fprintf(instr_set_fp, "MOV SP, %d\n", free_address);
    fprintf(instr_set_fp, "MOV BP, SP\n");
    int* free_registers = (int*) calloc(NUM_REGISTERS, sizeof(int));
    reset_registers(free_registers);
    
    
    
    global_symbol_table_t* main_entry = global_symbol_table_lookup("main");
    label_index_t main_label = main_entry->binding;

    save_machine_state(free_registers);
    fprintf(instr_set_fp, "PUSH R0\n");
    fprintf(instr_set_fp, "CALL L%d\n", main_label);
    fprintf(instr_set_fp, "POP R0\n");
    restore_machine_state(free_registers);
    exit_program(free_registers);

    generate_program_structure(body);
    generate_class_code();

    class_stack_destroy(class_stack_top());
    activation_stack_destroy(activation_stack_top());
}