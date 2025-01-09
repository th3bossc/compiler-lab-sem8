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
    // ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_READ, var_node, NULL);
    ast_node_t* node = create_node(data, default_types->int_type, NODE_TYPE_READ, expr, NULL, NULL);
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
}


ast_node_t* create_connector_node(ast_node_t* left, ast_node_t* right) {
    node_value_t data;
    ast_node_t* node = create_node(data, default_types->void_type, NODE_TYPE_CONNECTOR, left, right, NULL);
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

void destroy_node(ast_node_t* node) {
    if (node->left)
        destroy_node(node->left);
    if (node->right)
        destroy_node(node->right);
    if (node->middle)
        destroy_node(node->middle);

    free(node);
}


// int evaluate_expression(ast_node_t* node, int* vars) {
//     if (node->value_type == NODE_VALUE_INT) {
//         return evaluate_int_expression(node, vars);
//     }
//     else if (node->value_type == NODE_VALUE_BOOL) {
//         return evaluate_bool_expression(node, vars);
//     }
//     else if (node->value_type == NODE_VALUE_NOT_SET) {
//         ast_node_t* lval = node->left;
//         ast_node_t* rval = node->right;
//         if (!lval || !rval) {
//             perror("node:evaluate_expression: expr node doesn't have both left and right subtrees");
//             exit(1);
//         }

//         if (lval->value_type != rval->value_type) {
//             perror("node:evaluate_expression: mismatched types");
//             exit(1);
//         }

//         if (lval->value_type == NODE_VALUE_INT) {
//             node->value_type = NODE_VALUE_INT;
//             return evaluate_int_expression(node, vars);
//         }
//         else if (lval->value_type == NODE_VALUE_BOOL) {
//             node->value_type = NODE_VALUE_BOOL;
//             return evaluate_bool_expression(node, vars);
//         }
//     }
// }


// int evaluate_int_expression(ast_node_t* node, int* vars) {
//     if (!node)
//         return 0;

//     if (node->node_type == NODE_TYPE_INT)
//         return node->data.val;
//     else if (node->node_type == NODE_TYPE_ID)
//         return vars[node->data.var_name - 'a'];
//     else if (node->node_type == NODE_TYPE_OPERATOR) {
//         char op = node->data.op;
//         int l_val = evaluate_expression(node->left, vars);
//         int r_val = evaluate_expression(node->right, vars);
//         int answer;
//         switch (op) {
//             case '+':
//                 answer = l_val + r_val;
//                 break;
//             case '-':
//                 answer = l_val - r_val;
//                 break;
//             case '*':
//                 answer = l_val * r_val;
//                 break;
//             case '/':
//                 answer = l_val / r_val;
//                 break;
//         }
//         return answer;
//     }
// }

// int evaluate_bool_expression(ast_node_t* node, int* vars) {
//     if (!node)
//         return 0;

//     if (node->node_type == NODE_TYPE_INT)
//         return node->data.val;
//     else if (node->node_type == NODE_TYPE_ID)
//         return vars[node->data.var_name - 'a'];

//     char relop[2];
//     strcpy(relop, node->data.relop);
//     int lval = evaluate_expression(node->left, vars);
//     int rval = evaluate_expression(node->right, vars);

//     if (strcmp(relop, ">") == 0 && lval > rval) {
//         return true;
//     }
//     else if (strcmp(relop, ">=") == 0 && (lval >= rval)) {
//         return true;
//     } 
//     else if (strcmp(relop, "<") == 0 && (lval < rval)) {
//         return true;
//     } 
//     else if (strcmp(relop, "<=") == 0 && (lval <= rval)) {
//         return true;
//     } 
//     else if (strcmp(relop, "==") == 0 && (lval == rval)) {
//         return true;
//     } 
//     else if (strcmp(relop, "!=") == 0 && (lval != rval)) {
//         return true;
//     }  
//     else {
//         return false;
//     }
// }

// void evaluate_helper(ast_node_t* node, int* vars) {
//     if (!node)
//         return;

//     if (node->node_type == NODE_TYPE_CONNECTOR) {
//         evaluate_helper(node->left, vars);
//         evaluate_helper(node->right, vars);
//     }
//     else if (node->node_type == NODE_TYPE_WRITE) {
//         int expr_output = evaluate_expression(node->left, vars);
//         printf("%d\n", expr_output);
//     }
//     else if (node->node_type == NODE_TYPE_READ) {
//         int input;
//         scanf("%d", &input);
//         vars[node->left->data.var_name - 'a'] = input;
//     }
//     else if (node->node_type == NODE_TYPE_ASSIGN) {
//         int expr_output = evaluate_expression(node->right, vars);
//         vars[node->left->data.var_name - 'a'] = expr_output;
//     }
//     else if (node->node_type == NODE_TYPE_IF) {
//         if (node->left->value_type != NODE_VALUE_BOOL) {
//             perror("{node:evaluate_helper} IF condn is not a boolean value");
//             exit(1);
//         }
//         int cond_output = evaluate_expression(node->left, vars);
//         if (cond_output) {
//             evaluate_helper(node->right, vars);
//         }
//     }
//     else if (node->node_type == NODE_TYPE_IFELSE) {
//         ast_node_t* if_node = node->left;
//         if (if_node->left->value_type != NODE_VALUE_BOOL) {
//             perror("{node:evaluate_helper} IFELSE condn is not a boolean value");
//             exit(1);
//         }
//         int cond_output = evaluate_expression(if_node->left, vars);
//         if (cond_output) {
//             evaluate_helper(if_node->right, vars);
//         }
//         else {
//             evaluate_helper(node->right, vars);
//         }
//     }
//     else if (node->node_type == NODE_TYPE_WHILE) {
//         if (node->left->value_type != NODE_VALUE_BOOL) {
//             perror("{node:evaluate_helper} WHILE condn is not a boolean value");
//             exit(1);
//         }
//         while(evaluate_expression(node->left, vars)) {
//             evaluate_helper(node->right, vars);
//         }
//     }
//     else if (node->node_type == NODE_TYPE_OPERATOR || node->node_type == NODE_TYPE_INT || node->node_type == NODE_TYPE_ID || node->node_type == NODE_TYPE_RELOP)
//         return;
// }


// void evaluate_node(ast_node_t* node) {
//     int vars[26];
//     evaluate_helper(node, vars);
// }

void print_prefix(ast_node_t* node) {
    if (!node)
        return;
    printf("( ");
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
        case NODE_TYPE_STRING:
            printf("%s ", node->data.s_val);
        case NODE_TYPE_ASSIGN:
        case NODE_TYPE_OPERATOR:
            printf("%c ", node->data.c_val);
            break;
        case NODE_TYPE_IFELSE:
            printf("IFELSE ");
            break;
        case NODE_TYPE_IF:
            printf("IF ");
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
        case NODE_TYPE_RELOP:
            printf("%s ", node->data.s_val);
            break;
    }

    print_prefix(node->left);
    print_prefix(node->right);
    printf(") ");
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