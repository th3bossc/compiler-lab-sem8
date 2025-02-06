#include "ast_node.h"

// int evaluate_expression(ast_node_t* node, int* vars);
// int evaluate_bool_expression(ast_node_t* node, int* vars);
// int evaluate_int_expression(ast_node_t* node, int* vars);

ast_node_t* create_node(node_value_t data, type_table_t* value_type, node_type_t node_type, ast_node_t* left, ast_node_t* right, ast_node_t* middle) {
    ast_node_t* node = (ast_node_t*) malloc(sizeof(ast_node_t));
    node->data = data;
    node->value_type = value_type;
    node->node_type = node_type;
    node->left = left;
    node->right = right;
    node->middle = middle;

    return node;
}


ast_node_t* create_num_node(int val) {
    node_value_t data;
    data.n_val = val;
    ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_INT, NULL, NULL, NULL);
    return node;
}

ast_node_t* create_string_node(char* literal) {
    node_value_t data;
    data.s_val = strdup(literal);
    ast_node_t* node = create_node(data, default_types->str_type, NODE_TYPE_STRING, NULL, NULL, NULL);
    return node;
}

ast_node_t* create_id_node(char* var_name) {
    node_value_t data;
    data.s_val = strdup(var_name);
    ast_node_t* node = create_node(data, default_types->unset_type, NODE_TYPE_ID, NULL, NULL, NULL);
    return node;
}

ast_node_t* create_tuple_field_node(ast_node_t* addr_node, char* field_name) {
    node_value_t data;
    ast_node_t* field_node = create_id_node(field_name);
    ast_node_t* node = create_node(data, default_types->unset_type, NODE_TYPE_TUPLE_FIELD, addr_node, field_node, NULL);
    return node;
}

ast_node_t* create_operator_node(char op, ast_node_t* left, ast_node_t* right) {
    node_value_t data;
    data.c_val = op;

    ast_node_t* node = create_node(data, default_types->unset_type, NODE_TYPE_OPERATOR, left, right, NULL);
    return node;
} 

ast_node_t* create_write_node(ast_node_t* expr) {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_WRITE, expr, NULL, NULL);
    return node;
}

ast_node_t* create_read_node(ast_node_t* expr) {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_READ, expr, NULL, NULL);
    return node;
}

ast_node_t* create_init_heap_node() {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_INIT_HEAP, NULL, NULL, NULL);
    return node;
}

ast_node_t* create_alloc_node() {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_ALLOC, NULL, NULL, NULL);
    return node;
}

ast_node_t* create_free_node(char* var_name) {
    node_value_t data;
    ast_node_t* id_node = create_id_node(var_name);
    ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_FREE, NULL, NULL, id_node);
    return node;
}

ast_node_t* create_arr_index_node(char* arr_name, ast_node_t* index_node) {
    node_value_t data;
    ast_node_t* id_node = create_id_node(arr_name);
    ast_node_t* node = create_node(data, default_types->unset_type, NODE_TYPE_ARR_INDEX, id_node, index_node, NULL);
    return node;
}

ast_node_t* create_2d_arr_index_node(char* arr_name, ast_node_t* outer_index_node, ast_node_t* inner_index_node) {
    node_value_t data;
    ast_node_t* id_node = create_id_node(arr_name);
    global_symbol_table_t* var_details = global_symbol_table_lookup(arr_name);
    if (var_details == NULL) {
        yyerror("{ast_node:create_2d_arr_index_node} Variable not declared");
        exit(1);
    }
    int row_size = var_details->inner_size;
    ast_node_t* row_number_index_node = create_operator_node('*', outer_index_node, create_num_node(row_size));
    ast_node_t* index_node = create_operator_node('+', row_number_index_node, inner_index_node);
    ast_node_t* node = create_node(data, default_types->unset_type, NODE_TYPE_ARR_INDEX, id_node, index_node, NULL);
    return node;
}

ast_node_t* create_ptr_deref_node(ast_node_t* expr) {
    node_value_t data;
    data.c_val = '&';
    ast_node_t* node = create_node(data, default_types->ptr_type, NODE_TYPE_PTR_DEREF, expr, NULL, NULL);
    return node;
}

ast_node_t* create_ptr_ref_node(ast_node_t* expr) {
    node_value_t data;
    data.c_val = '*';
    ast_node_t* node = create_node(data, default_types->unset_type, NODE_TYPE_PTR_REF, expr, NULL, NULL);
    return node;
}


ast_node_t* create_connector_node(ast_node_t* left, ast_node_t* right) {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_CONNECTOR, left, right, NULL);
    return node;
}

ast_node_t* create_program_node(ast_node_t* left, ast_node_t* right) {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_PROGRAM, left, right, NULL);
    return node;
}

ast_node_t* create_assignment_node(char* var_name, ast_node_t* expr) {
    node_value_t data;
    data.c_val = '=';
    ast_node_t* id_node = create_id_node(var_name);
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_ASSIGN, id_node, expr, NULL);
    return node;
}

ast_node_t* create_arr_assignment_node(char* var_name, ast_node_t* index_node, ast_node_t* expr) {
    node_value_t data;
    data.c_val = '=';
    ast_node_t* arr_node = create_arr_index_node(var_name, index_node);
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_ASSIGN, arr_node, expr, NULL);
    return node;
}

ast_node_t* create_2d_arr_assignment_node(char* var_name, ast_node_t* outer_index_node, ast_node_t* inner_index_node, ast_node_t* expr) {
    node_value_t data;
    data.c_val = '=';
    ast_node_t* arr_node = create_2d_arr_index_node(var_name, outer_index_node, inner_index_node);
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_ASSIGN, arr_node, expr, NULL);
    return node;
}

ast_node_t* create_ptr_assignment_node(char* var_name, ast_node_t* expr) {
    node_value_t data;
    data.c_val = '=';
    ast_node_t* id_node = create_id_node(var_name);
    ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_PTR_REF, id_node, expr, NULL);
    return node;
}

ast_node_t* create_tuple_field_assignment_node(ast_node_t* addr_node, char* field_name, ast_node_t* expr) {
    node_value_t data;
    data.c_val = '=';
    ast_node_t* id_node = create_tuple_field_node(addr_node, field_name);
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_ASSIGN, id_node, expr, NULL);
    return node;
}

ast_node_t* create_relop_node(char relop[], ast_node_t* left, ast_node_t* right) {
    node_value_t data;
    data.s_val = strdup(relop);
    ast_node_t* node = create_node(data, default_types->bool_type, NODE_TYPE_RELOP, left, right, NULL);
    return node;
}

ast_node_t* create_ifelse_node(ast_node_t* condn, ast_node_t* if_subtree, ast_node_t* else_subtree) {
    node_value_t data;
    node_type_t node_type = (else_subtree == NULL) ? NODE_TYPE_IF : NODE_TYPE_IFELSE;
    ast_node_t* if_node = create_node(data, default_types->void_type, node_type, if_subtree, else_subtree, condn);
    return if_node;
}

ast_node_t* create_while_node(ast_node_t* condn, ast_node_t* body) {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_WHILE, condn, body, NULL);
    return node;
}

ast_node_t* create_do_while_node(ast_node_t* condn, ast_node_t* body) {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_DO_WHILE, condn, body, NULL);
    return node;
}

ast_node_t* create_repeat_node(ast_node_t* condn, ast_node_t* body) {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_REPEAT, condn, body, NULL);
    return node;
}

ast_node_t* create_func_call_node(char* func_name, args_node_t* args_list) {
    node_value_t data;
    data.s_val = strdup(func_name);
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_FUNC_CALL, NULL, NULL, NULL);
    node->args_list = args_list;
    return node;
}

ast_node_t* create_func_body_node(char* func_name, type_table_t* return_type, decl_node_t* params_list, decl_node_t* local_decls, ast_node_t* body) {
    node_value_t data;
    data.s_val = strdup(func_name);
    global_symbol_table_t* entry = global_symbol_table_lookup(func_name);
    if (!entry || (entry->type != default_types->func_type)) {
        yyerror("{ast_node:create_func_body_node} the variable is not a function");
        exit(1);
    } 

    if (entry->inner_type != return_type) {
        yyerror("{ast_node:create_func_body_node} return types don't match");
        exit(1);
    }

    if (!verify_params_list(params_list, entry->params)) {
        yyerror("{ast_node:create_func_body_node} params of declaration and defintion of function doesn't match");
        exit(1);
    }
    entry->local_decls = local_decls;
    entry->binding = get_label();
    ast_node_t* node = create_node(data, entry->type, NODE_TYPE_FUNC_DECL, NULL, NULL, body);
    node->func_details = entry;
    return node;
}


ast_node_t* create_func_return_node(ast_node_t* expr) {
    node_value_t data;
    data.s_val = "return";
    ast_node_t* node = create_node(data, default_types->unset_type, NODE_TYPE_FUNC_RET, NULL, NULL, expr);
    return node;
}


ast_node_t* create_break_node() {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_BREAK, NULL, NULL, NULL);
    return node;
}

ast_node_t* create_continue_node() {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_CONTINUE, NULL, NULL, NULL);
    return node;
}


ast_node_t* create_self_node() {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->unset_type, NODE_TYPE_SELF, NULL, NULL, NULL);
    return node;
}

void destroy_node(ast_node_t* node) {
    if (node->left)
        destroy_node(node->left);
    if (node->right)
        destroy_node(node->right);
    if (node->middle)
        destroy_node(node->middle);

    free(node);
}

void print_prefix(ast_node_t* node) {
    if (node == NULL)
        return;

    switch (node->node_type) {
        case NODE_TYPE_WRITE:
            printf("WRITE ");
            break;
        case NODE_TYPE_READ:
            printf("READ ");
            break;
        case NODE_TYPE_INIT_HEAP:
            printf("INITIALIZE ");
            break;
        case NODE_TYPE_ALLOC:
            printf("ALLOC ");
            break;
        case NODE_TYPE_FREE:
            printf("FREE ");
            break;
        case NODE_TYPE_CONNECTOR:
            printf("CONNECTOR ");
            break;
        case NODE_TYPE_ID:
            printf("%s ", node->data.s_val);
            break;
        case NODE_TYPE_TUPLE_FIELD:
            printf(". ");
            break;
        case NODE_TYPE_INT:
            printf("%d ", node->data.n_val);
            break;
        case NODE_TYPE_STRING:
            printf("%s ", node->data.s_val);
            break;
        case NODE_TYPE_ASSIGN:
        case NODE_TYPE_OPERATOR:
            printf("%c ", node->data.c_val);
            break;
        case NODE_TYPE_RELOP:
            printf("%s ", node->data.s_val);
            break;
        case NODE_TYPE_IF:
            printf("IF ");
            break;
        case NODE_TYPE_IFELSE:
            printf("IFELSE ");
            break;
        case NODE_TYPE_WHILE:
            printf("WHILE ");
            break;
        case NODE_TYPE_DO_WHILE:
            printf("DOWHILE ");
            break;
        case NODE_TYPE_REPEAT:
            printf("REPEATUNTIL");
            break;
        case NODE_TYPE_BREAK:
            printf("BREAK ");
            break;
        case NODE_TYPE_CONTINUE:
            printf("CONTINUE ");
            break;
        case NODE_TYPE_ARR_INDEX:
            printf("ARR_INDEX ");
            break;
        case NODE_TYPE_PTR_REF:
            printf("PTR REF");
            break;
        case NODE_TYPE_PTR_DEREF:
            printf("PTR DEREF ");
            break;
        case NODE_TYPE_FUNC_CALL:
            printf("CALL FUNC %s ", node->data.s_val);
            break;
        case NODE_TYPE_FUNC_DECL:
            printf("DECL FUNC %s ", node->data.s_val);
            break;
        case NODE_TYPE_PROGRAM:
            printf("PGM CONNECTOR ");
            break;
        case NODE_TYPE_FUNC_RET:
            printf("RET ");
            break;
    }

    print_prefix(node->left);
    print_prefix(node->middle);
    print_prefix(node->right);
    // printf(") ");
}

void print_postfix(ast_node_t* node) {
    if (!node)
        return;

    print_postfix(node->left);
    print_postfix(node->right);

    switch (node->node_type) {
        case NODE_TYPE_CONNECTOR:
            printf("CONNECTOR ");
            break;
        case NODE_TYPE_WRITE:
            printf("WRITE ");
            break;
        case NODE_TYPE_READ:
            printf("READ ");
            break;
        case NODE_TYPE_ID:
            printf("%s ", node->data.s_val);
            break;
        case NODE_TYPE_INT:
            printf("%d ", node->data.n_val);
            break;
        case NODE_TYPE_ASSIGN:
        case NODE_TYPE_OPERATOR:
            printf("%c ", node->data.c_val);
            break;
    }
}