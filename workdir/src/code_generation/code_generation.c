#include "code_generation.h"

type_table_t* is_type_compatible(type_table_t* type1, type_table_t* type2) {
    if ((is_user_defined_type(type1) || is_class(type1)) && (type2 == default_types->int_type)) 
        return type1;
    else if ((is_user_defined_type(type2) || is_class(type2)) && (type1 == default_types->int_type))
        return type2;
    if (type1 == type2)
        return type1;
    else if (type1 == default_types->int_type && type2 == default_types->ptr_type)
        return type2;
    else if (type1 == default_types->ptr_type && type2 == default_types->int_type)
        return type1;
    else if (type1 == default_types->unset_type || type2 == default_types->unset_type)
        return type1;
    else if (is_class(type1) && is_class(type2)) {
        if (is_descendant_of(type1->class_details, type2->class_details))
            return type1;
    }
    return NULL;
}

reg_index_t load_var_data_to_reg(char* name, reg_index_t offset_reg) {
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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



reg_index_t load_class_details(ast_node_t* id_node) {
    function_metadata_t* record = function_metdata_current();
    if (id_node->node_type == NODE_TYPE_TUPLE_FIELD) {

        reg_index_t id_addr = generate_expression_code(id_node->left);
        type_table_t* id_type = id_node->left->value_type;
        if (!is_user_defined_type(id_type) && !is_class(id_type)) {
            yyerror("{code_generation:load_class_details} '.' operator is only permitted on user defined types");
            exit(1);
        }

        if (is_user_defined_type(id_type)) {
            ast_node_t* field_node = id_node->right;
            field_list_t* field_info = field_lookup(id_type, field_node->data.s_val);
            if (field_info == NULL) {
                yyerror("{code_generation:load_class_details} field doesn't exist on type");
                exit(1);
            }
            type_table_t* field_type = field_info->type;
            field_node->value_type = field_type;
            id_node->value_type = field_type;
            reg_index_t field_index_reg = get_free_register(record->num_used_regs);
            immediate_addressing_int(field_index_reg, field_info->field_index);
            add_registers(id_addr, field_index_reg);
            free_used_register(record->num_used_regs, field_index_reg);


            if (!is_class(field_type)) {
                yyerror("{code_generation:load_class_details} 'new' operator can only be used on an instance of a class");
                exit(1);
            }

            return id_addr;
        }
        else {
            ast_node_t* field_node = id_node->right;
            class_field_t* field_info = class_field_lookup(id_type->class_details, field_node->data.s_val);
            if (field_info == NULL) {
                yyerror("{code_generation:generate_class_constructor_code} field doesn't exist on type");
                exit(1);
            }

            type_table_t* field_type = field_info->type;
            field_node->value_type = field_type;
            id_node->value_type = field_type;
            

            reg_index_t field_index_reg = get_free_register(record->num_used_regs);
            immediate_addressing_int(field_index_reg, field_info->field_index);
            add_registers(id_addr, field_index_reg);
            free_used_register(record->num_used_regs, field_index_reg);

            if (!is_class(field_type)) {
                yyerror("{code_generation:generate_class_constructor_code} 'new' operator can only be used on an instance of a class");
                exit(1);
            }

            return id_addr;
        }
    }
    else if (id_node->node_type == NODE_TYPE_ARR_INDEX) {
        ast_node_t* arr_node = id_node->left;
        arr_node->value_type = get_var_type(arr_node->data.s_val, record->l_symbol_table);
        if ((arr_node->node_type != NODE_TYPE_ID) || (arr_node->value_type != default_types->arr_type)) {
            yyerror("{code_generation:generate_arr_assignment_code}: indexing can only be done on arrays");
            exit(1);
        }

        ast_node_t* index_node = id_node->right;
        reg_index_t index_output = generate_expression_code(index_node);
        if (index_node->value_type != default_types->int_type) {
            yyerror("{code_generation:generate_arr_assignment_code}: arr index should be an int value");
            exit(1);
        }

        id_node->value_type = get_var_inner_type(arr_node->data.s_val, record->l_symbol_table);
        mul_immediate(index_output, id_node->value_type->size);
        if (!is_class(id_node->value_type)) {
            yyerror("{code_generation:load_class_details} Type mismatch");
            exit(1);
        }
        reg_index_t arr_index_loc = load_var_addr_to_reg(arr_node->data.s_val, index_output);
        free_used_register(record->num_used_regs, index_output);
        return arr_index_loc;
    }
    else {
        type_table_t* id_type = get_var_type(id_node->data.s_val, record->l_symbol_table);
        id_node->value_type = id_type;

        if (!is_class(id_type)) {
            yyerror("{code_generation:generate_class_constructor_code} 'new' operator can only be used on an instance of a class");
            exit(1);
        }

        reg_index_t free_reg = load_var_addr_to_reg(id_node->data.s_val, -1);
        return free_reg;
    }
}


reg_index_t generate_id_expr_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
    if (!node) {
        yyerror("{code_generation:generate_id_expr_code} Something went wrong");
        exit(1);
    }

    type_table_t* id_type = get_var_type(node->data.s_val, record->l_symbol_table);
    node->value_type = id_type;
    if (is_tuple(id_type)) {
        reg_index_t id_val = load_var_addr_to_reg(node->data.s_val, -1);
        return id_val;
    }
    else {
        reg_index_t id_val = load_var_data_to_reg(node->data.s_val, -1);
        return id_val;
    }
}

reg_index_t generate_expression_code(ast_node_t* node) {
    if (node == NULL) {
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
    else if (node->node_type == NODE_TYPE_CLASS_METHOD) {
        return generate_class_method_call_code(node);
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_arr_index_code}: Something went wrong.");
        exit(1);
    }

    ast_node_t* id_node = node->left;
    if ((id_node->node_type != NODE_TYPE_ID) || get_var_type(id_node->data.s_val, record->l_symbol_table) != default_types->arr_type) {
        yyerror("{code_generation:generate_arr_index_code}: indexing can only be done on arrays");
        exit(1);
    }
    node->value_type = get_var_inner_type(id_node->data.s_val, record->l_symbol_table);

    ast_node_t* index_node = node->right;
    reg_index_t expr_output = generate_expression_code(index_node);
    if (index_node->value_type != default_types->int_type) {
        yyerror("{code_generation:generate_arr_index_code}: arr index should be an int value");
        exit(1);
    }

    mul_immediate(expr_output, node->value_type->size);
    reg_index_t arr_data = load_var_data_to_reg(id_node->data.s_val, expr_output);
    free_used_register(record->num_used_regs, expr_output);
    return arr_data;
}

reg_index_t generate_arithmetic_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
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
    printf("type1: %d, type2: %d\n", node->left->node_type, node->right->node_type);
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_tuple_index_code} Something went wrong");
        exit(1);
    }

    reg_index_t id_addr = generate_expression_code(node->left);
    type_table_t* id_type = node->left->value_type;

    if (!is_user_defined_type(id_type) && !is_class(id_type)) {
        yyerror("{code_generation:generate_tuple_index_code} '.' operator can only be used on user defined types");
        exit(1);
    }

    if (!is_class(id_type)) {
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
    else {
        ast_node_t* field_node = node->right;
        class_field_t* field_info = class_field_lookup(id_type->class_details, field_node->data.s_val);
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
}


reg_index_t generate_ptr_ref_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
    return initialize_heap_code(record->num_used_regs);
}

reg_index_t generate_alloc_code() {
    return alloc_memory(8);
}

reg_index_t generate_free_heap_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
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

reg_index_t generate_class_method_call_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
    if (!node || !node->left || !node->right) {
        yyerror("{code_generation:generate_class_method_call_code} Something went wrong");
        exit(1);
    }

    reg_index_t id_addr = load_class_details(node->left);
    reg_index_t id_expr = get_free_register(record->num_used_regs);
    register_indirect_addressing_load(id_expr, id_addr);
    reg_index_t virt_func_table_base = get_free_register(record->num_used_regs);
    register_addressing(virt_func_table_base, id_addr);
    incr_register(virt_func_table_base);
    register_indirect_addressing_load(virt_func_table_base, virt_func_table_base);
    if (!is_class(node->left->value_type)) {
        yyerror("{code_generation:generate_class_method_call_code} 'ID' not callable");
        exit(1);
    }

    class_table_t* class_details = node->left->value_type->class_details;

    args_node_t* it2 = node->right->args_list;
    int num_args = count_num_args(it2);
    reg_index_t values[num_args];

    for (int i = 0; i < num_args; i++) {
        values[i] = generate_expression_code(it2->expr_node);
        it2 = it2->next;
    }


    class_method_t* method_entry = class_method_lookup_via_args(class_details, node->right->data.s_val, node->right->args_list);
    if (method_entry == NULL) {
        yyerror("{code_generation:generate_class_method_call_code} Method overload isn't present on class type");
        exit(1);
    }

    int method_index = method_entry->method_index;

    incr_register(id_addr);
    reg_index_t virt_func_table = get_free_register(record->num_used_regs);
    register_indirect_addressing_load(virt_func_table, id_addr);

    add_immediate(virt_func_table, method_index);
    reg_index_t label_reg = get_free_register(record->num_used_regs);
    register_indirect_addressing_load(label_reg, virt_func_table);

    free_used_register(record->num_used_regs, virt_func_table);

    node->value_type = method_entry->return_type;
    // label_index_t func_label = method_entry->label;
    save_machine_state(record->num_used_regs);

    decl_node_t* it1 = method_entry->params;

    // while(it1 != NULL && it2 != NULL) {
    //     reg_index_t arg_expr_output = generate_expression_code(it2->expr_node);
    //     if (it1->type != it2->expr_node->value_type) {
    //         print_prefix(it2->expr_node);
    //         yyerror("{code_generation:generate_class_method_call_code} Type mismatch in calling function");
    //         exit(1);
    //     }

    //     push_register(arg_expr_output);
    //     free_used_register(record->num_used_regs, arg_expr_output);

    //     it1 = it1->next;
    //     it2 = it2->next;
    // }

    for (int i = 0; i < num_args; i++) {
        reg_index_t args_expr_output = values[i];
        push_register(args_expr_output);
        free_used_register(record->num_used_regs, args_expr_output);
    }

    push_register(id_expr);
    push_register(virt_func_table_base);


    // if (it1 != NULL || it2 != NULL) {
    //     yyerror("{code_generation:generate_class_method_call_code} Parameter count doesn't match");
    //     exit(1);
    // }

    push_register(0);
    fprintf(instr_set_fp, "CALL R%d\n", label_reg);
    pop_register(0);

    it1 = method_entry->params;
    reg_index_t free_reg = get_free_register(record->num_used_regs);
    while (it1 != NULL) {
        pop_register(free_reg);
        it1 = it1->next;
    }
    pop_register(free_reg);
    pop_register(free_reg);
    free_used_register(record->num_used_regs, free_reg);
    restore_machine_state(record->num_used_regs);
    free_used_register(record->num_used_regs, id_addr);
    free_used_register(record->num_used_regs, id_expr);
    free_used_register(record->num_used_regs, label_reg);
    free_used_register(record->num_used_regs, virt_func_table_base);
    reg_index_t ret_val = get_free_register(record->num_used_regs);
    register_addressing(ret_val, RESERVED_RETURN_REG);
    return ret_val;
}


void generate_assignment_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
    if (!node || node->node_type != NODE_TYPE_ASSIGN || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_assignment_code} Something went wrong");
        exit(1);
    }

    if (node->right->node_type == NODE_TYPE_CONSTRUCTOR) {
        return generate_class_constructor_code(node);
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
    else if (is_class(output_type)) {
        reg_index_t src_class_details = load_class_details(node->right);
        reg_index_t dest_class_details = load_class_details(node->left);
        reg_index_t free_reg = get_free_register(record->num_used_regs);

        register_indirect_addressing_load(free_reg, src_class_details);
        register_indirect_addressing_store(dest_class_details, free_reg);
        incr_register(src_class_details);
        incr_register(dest_class_details);
        register_indirect_addressing_load(free_reg, src_class_details);
        register_indirect_addressing_store(dest_class_details, free_reg);

        free_used_register(record->num_used_regs, src_class_details);
        free_used_register(record->num_used_regs, dest_class_details);
        free_used_register(record->num_used_regs, free_reg);
    }
    else {
        ast_node_t* id_node = node->left;
        reg_index_t free_reg = load_var_addr_to_reg(id_node->data.s_val, -1);
        register_indirect_addressing_store(free_reg, expr_output);
        free_used_register(record->num_used_regs, free_reg);
        free_used_register(record->num_used_regs, expr_output);
    }
}
 
void generate_class_constructor_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_class_constructor_code} Something went wrong");
        exit(1);
    }

    class_table_t* class_details = class_table_lookup(node->right->data.s_val);
    if (class_details == NULL) {
        yyerror("{code_generation:generate_class_constructor_code} Class doesn't exist");
        exit(1);
    }
    reg_index_t field_block = generate_alloc_code();
    reg_index_t field_addr = load_class_details(node->left);

    if (!is_descendant_of(node->left->value_type->class_details, class_details)) {
        yyerror("{code_generation:generate_class_constructor_code} 'new' operator can only be used on same or related classes");
        exit(1);
    }


    register_indirect_addressing_store(field_addr, field_block);
    incr_register(field_addr);

    int virt_func_table = 4096 + (class_details->class_index)*8;
    reg_index_t func_table_reg = get_free_register(record->num_used_regs);
    immediate_addressing_int(func_table_reg, virt_func_table);

    register_indirect_addressing_store(field_addr, func_table_reg);
    free_used_register(record->num_used_regs, field_block);
    free_used_register(record->num_used_regs, field_addr);
    free_used_register(record->num_used_regs, func_table_reg);
}

void generate_arr_assignment_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
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
    mul_immediate(index_output, arr_index_node->value_type->size);
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
    if (!node || !(node->left) || !(node->right)) {
        yyerror("{code_generation:generate_tuple_field_assignment_code} Something went wrong");
        exit(1);
    }

    ast_node_t* id_node = node->left;
    ast_node_t* expr_node = node->right;

    reg_index_t id_addr = generate_expression_code(id_node->left);
    type_table_t* id_type = id_node->left->value_type;
    if (!is_user_defined_type(id_type) && !is_class(id_type)) {
        yyerror("{code_generation:generate_tuple_field_assignment_code} '.' operator is only permitted on user defined types");
        exit(1);
    }

    if (is_user_defined_type(id_type)) {
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
        reg_index_t expr_output = generate_expression_code(expr_node);

        type_table_t* output_type = is_type_compatible(expr_node->value_type, field_node->value_type);
        if (output_type == NULL) {
            yyerror("{code_generation:generate_tuple_field_assignment_code} type mismatch");
            exit(1);
        }
        node->value_type = default_types->void_type;
        if (output_type->size == 1 || is_primitive_type(output_type)) {
            register_indirect_addressing_store(id_addr, expr_output);
        }
        else {
            if (!is_class(output_type)) {
                for (int i = 0; i < output_type->size; i++) {
                    register_indirect_addressing_store(id_addr, expr_output);
                    incr_register(id_addr);
                    incr_register(expr_output);
                }
            }
            else {
                reg_index_t src_class_details = load_class_details(expr_node);
                reg_index_t free_reg = get_free_register(record->num_used_regs);

                register_indirect_addressing_load(free_reg, src_class_details);
                register_indirect_addressing_store(id_addr, free_reg);
                incr_register(id_addr);
                incr_register(src_class_details);
                register_indirect_addressing_load(free_reg, src_class_details);
                register_indirect_addressing_store(id_addr, free_reg);

                free_used_register(record->num_used_regs, src_class_details);
                free_used_register(record->num_used_regs, free_reg);
            }
        }
        free_used_register(record->num_used_regs, id_addr);
        free_used_register(record->num_used_regs, expr_output);
    }
    else {
        ast_node_t* field_node = id_node->right;
        class_field_t* field_info = class_field_lookup(id_type->class_details, field_node->data.s_val);
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
}

reg_index_t generate_print_code(ast_node_t* node) {
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
        if (!is_user_defined_type(id_type) && !is_class(id_type)) {
            yyerror("{code_generation:generate_read_code} '.' operator is only permitted on user defined types");
            exit(1);
        }

        if (is_user_defined_type(id_type)) {
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
        else {
            ast_node_t* field_node = id_node->right;
            class_field_t* field_info = class_field_lookup(id_type->class_details, field_node->data.s_val);
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
}

void generate_if_code(ast_node_t* node, label_index_t* break_label, label_index_t* continue_label) {
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
        i += entry->type->size;
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
        i += entry->type->size;
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

    function_metadata_update_func(func_entry, local_symbol_table, num_used_registers);
    generate_statement_structure(node->middle, NULL, NULL);
    
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
        i += entry->type->size;
    }


    // creating local decl for 'self' keyword
    local_symbol_table_t* new_entry = create_local_symbol_table_entry(
        "self",
        get_type_table_entry(method->class->name),
        NULL,
        i,
        NULL
    );
    local_symbol_table = append_to_local_table(local_symbol_table, new_entry);
    i += 2;
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
        i += entry->type->size;
    }

    print_prefix(method->func_body);
    
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

        free_used_register(num_used_registers, dest_reg);
        free_used_register(num_used_registers, src_reg);
    }

    function_metadata_update_member_method(method, local_symbol_table, num_used_registers);
    generate_statement_structure(method->func_body, NULL, NULL);
    
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    if (node->node_type == NODE_TYPE_CLASS_METHOD) {
        reg_index_t ret_val = generate_class_method_call_code(node);
        free_used_register(record->num_used_regs, ret_val);
    }
}


reg_index_t print_register(reg_index_t data) {
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
    reg_index_t free_reg = get_free_register(record->num_used_regs);
    
    direct_addressing_load(addr, free_reg);
    reg_index_t ret_val = print_register(free_reg);
    free_used_register(record->num_used_regs, free_reg);

    return ret_val;
}

reg_index_t read_into_addr(int addr) {
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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
    function_metadata_t* record = function_metdata_current();
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

void generate_virtual_func_table(int* num_used_regs) {
    for (class_table_t* class_details = class_table; class_details != NULL; class_details = class_details->next) {
        int index = class_details->class_index;

        reg_index_t virtual_func_table = get_free_register(num_used_regs);
        immediate_addressing_int(virtual_func_table, 4096 + index*8);


        for (class_method_t* method = class_details->methods; method != NULL; method = method->next) {
            reg_index_t free_reg = get_free_register(num_used_regs);
            register_addressing(free_reg, virtual_func_table);
            add_immediate(free_reg, method->method_index);
            
            reg_index_t func_label = get_free_register(num_used_regs);
            fprintf(instr_set_fp, "MOV R%d, L%d\n", func_label, method->label);
            register_indirect_addressing_store(free_reg, func_label);

            free_used_register(num_used_regs, free_reg);
            free_used_register(num_used_regs, func_label);
        }

        free_used_register(num_used_regs, virtual_func_table);
    }
}


void generate_program(ast_node_t* body) {
    generate_headers();
    add_breakpoint();
    int free_address = get_binding(0);
    int* free_registers = (int*) calloc(NUM_REGISTERS, sizeof(int));
    reset_registers(free_registers);
    generate_virtual_func_table(free_registers);
    
    fprintf(instr_set_fp, "MOV SP, %d\n", free_address);
    fprintf(instr_set_fp, "MOV BP, SP\n");
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
}