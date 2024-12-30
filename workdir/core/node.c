#include "node.h"

int evaluate_expression(node_t* node, int* vars);
int evaluate_bool_expression(node_t* node, int* vars);
int evaluate_int_expression(node_t* node, int* vars);

node_t* create_node(node_value_t data, node_value_type_t value_type, node_type_t node_type, node_t* left, node_t* right) {
    node_t* node = (node_t*) malloc(sizeof(node_t));
    node->data = data;
    node->value_type = value_type;
    node->node_type = node_type;
    node->left = left;
    node->right = right;

    return node;
}


node_t* create_num_node(int val) {
    node_value_t data;
    data.val = val;

    node_t* node = create_node(data, NODE_VALUE_INT, NODE_TYPE_VALUE, NULL, NULL);
    return node;
}

node_t* create_id_node(char var_name) {
    node_value_t data;
    data.var_name = var_name;

    node_t* node = create_node(data, NODE_VALUE_INT, NODE_TYPE_ID, NULL, NULL);
    return node;
}

node_t* create_operator_node(char op, node_t* left, node_t* right) {
    node_value_t data;
    data.op = op;

    node_t* node = create_node(data, NODE_VALUE_NOT_SET, NODE_TYPE_OPERATOR, left, right);
    return node;
} 

node_t* create_write_node(node_t* expr) {
    node_value_t data;
    node_t* node = create_node(data, NODE_VALUE_INT, NODE_TYPE_WRITE, expr, NULL);
    return node;
}

node_t* create_read_node(char var_name) {
    node_t* var_node = create_id_node(var_name);
    node_value_t data;
    node_t* node = create_node(data, NODE_VALUE_INT, NODE_TYPE_READ, var_node, NULL);
    return node;
}

node_t* create_connector_node(node_t* left, node_t* right) {
    node_value_t data;
    node_t* node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_CONNECTOR, left, right);
    return node;
}

node_t* create_assignment_node(char var_name, node_t* expr) {
    node_value_t data;
    data.op = '=';
    node_t* id_node = create_id_node(var_name);
    node_t* node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_ASSIGN, id_node, expr);
    return node;
}

node_t* create_relop_node(char relop[], node_t* left, node_t* right) {
    node_value_t data;
    strcpy(data.relop, relop);
    node_t* node = create_node(data, NODE_VALUE_BOOL, NODE_TYPE_RELOP, left, right);
    return node;
}

node_t* create_ifelse_node(node_t* condn, node_t* if_subtree, node_t* else_subtree) {
    node_value_t data;
    node_t* if_node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_IF, condn, if_subtree);
    if (else_subtree == NULL) {
        return if_node;
    }
    else {
        node_t* connector_node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_IFELSE, if_node, else_subtree);
        return connector_node;
    }
}

node_t* create_while_node(node_t* condn, node_t* body) {
    node_value_t data;
    node_t* node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_WHILE, condn, body);
    return node;
}

node_t* create_do_while_node(node_t* condn, node_t* body) {
    node_value_t data;
    node_t* node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_DO_WHILE, condn, body);
    return node;
}

node_t* create_repeat_node(node_t* condn, node_t* body) {
    node_value_t data;
    node_t* node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_REPEAT, condn, body);
}

node_t* create_break_node() {
    node_value_t data;
    node_t* node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_BREAK, NULL, NULL);
    return node;
}

node_t* create_continue_node() {
    node_value_t data;
    node_t* node = create_node(data, NODE_VALUE_VOID, NODE_TYPE_CONTINUE, NULL, NULL);
    return node;
}

void destroy_node(node_t* node) {
    if (node->left)
        destroy_node(node->left);
    if (node->right)
        destroy_node(node->right);

    free(node);
}


int evaluate_expression(node_t* node, int* vars) {
    if (node->value_type == NODE_VALUE_INT) {
        return evaluate_int_expression(node, vars);
    }
    else if (node->value_type == NODE_VALUE_BOOL) {
        return evaluate_bool_expression(node, vars);
    }
    else if (node->value_type == NODE_VALUE_NOT_SET) {
        node_t* lval = node->left;
        node_t* rval = node->right;
        if (!lval || !rval) {
            perror("node:evaluate_expression: expr node doesn't have both left and right subtrees");
            exit(1);
        }

        if (lval->value_type != rval->value_type) {
            perror("node:evaluate_expression: mismatched types");
            exit(1);
        }

        if (lval->value_type == NODE_VALUE_INT) {
            node->value_type = NODE_VALUE_INT;
            return evaluate_int_expression(node, vars);
        }
        else if (lval->value_type == NODE_VALUE_BOOL) {
            node->value_type = NODE_VALUE_BOOL;
            return evaluate_bool_expression(node, vars);
        }
    }
}


int evaluate_int_expression(node_t* node, int* vars) {
    if (!node)
        return 0;

    if (node->node_type == NODE_TYPE_VALUE)
        return node->data.val;
    else if (node->node_type == NODE_TYPE_ID)
        return vars[node->data.var_name - 'a'];
    else if (node->node_type == NODE_TYPE_OPERATOR) {
        char op = node->data.op;
        int l_val = evaluate_expression(node->left, vars);
        int r_val = evaluate_expression(node->right, vars);
        int answer;
        switch (op) {
            case '+':
                answer = l_val + r_val;
                break;
            case '-':
                answer = l_val - r_val;
                break;
            case '*':
                answer = l_val * r_val;
                break;
            case '/':
                answer = l_val / r_val;
                break;
        }
        return answer;
    }
}

int evaluate_bool_expression(node_t* node, int* vars) {
    if (!node)
        return 0;

    if (node->node_type == NODE_TYPE_VALUE)
        return node->data.val;
    else if (node->node_type == NODE_TYPE_ID)
        return vars[node->data.var_name - 'a'];

    char relop[2];
    strcpy(relop, node->data.relop);
    int lval = evaluate_expression(node->left, vars);
    int rval = evaluate_expression(node->right, vars);

    if (strcmp(relop, ">") == 0 && lval > rval) {
        return true;
    }
    else if (strcmp(relop, ">=") == 0 && (lval >= rval)) {
        return true;
    } 
    else if (strcmp(relop, "<") == 0 && (lval < rval)) {
        return true;
    } 
    else if (strcmp(relop, "<=") == 0 && (lval <= rval)) {
        return true;
    } 
    else if (strcmp(relop, "==") == 0 && (lval == rval)) {
        return true;
    } 
    else if (strcmp(relop, "!=") == 0 && (lval != rval)) {
        return true;
    }  
    else {
        return false;
    }
}

void evaluate_helper(node_t* node, int* vars) {
    if (!node)
        return;

    if (node->node_type == NODE_TYPE_CONNECTOR) {
        evaluate_helper(node->left, vars);
        evaluate_helper(node->right, vars);
    }
    else if (node->node_type == NODE_TYPE_WRITE) {
        int expr_output = evaluate_expression(node->left, vars);
        printf("%d\n", expr_output);
    }
    else if (node->node_type == NODE_TYPE_READ) {
        int input;
        scanf("%d", &input);
        vars[node->left->data.var_name - 'a'] = input;
    }
    else if (node->node_type == NODE_TYPE_ASSIGN) {
        int expr_output = evaluate_expression(node->right, vars);
        vars[node->left->data.var_name - 'a'] = expr_output;
    }
    else if (node->node_type == NODE_TYPE_IF) {
        if (node->left->value_type != NODE_VALUE_BOOL) {
            perror("{node:evaluate_helper} IF condn is not a boolean value");
            exit(1);
        }
        int cond_output = evaluate_expression(node->left, vars);
        if (cond_output) {
            evaluate_helper(node->right, vars);
        }
    }
    else if (node->node_type == NODE_TYPE_IFELSE) {
        node_t* if_node = node->left;
        if (if_node->left->value_type != NODE_VALUE_BOOL) {
            perror("{node:evaluate_helper} IFELSE condn is not a boolean value");
            exit(1);
        }
        int cond_output = evaluate_expression(if_node->left, vars);
        if (cond_output) {
            evaluate_helper(if_node->right, vars);
        }
        else {
            evaluate_helper(node->right, vars);
        }
    }
    else if (node->node_type == NODE_TYPE_WHILE) {
        if (node->left->value_type != NODE_VALUE_BOOL) {
            perror("{node:evaluate_helper} WHILE condn is not a boolean value");
            exit(1);
        }
        while(evaluate_expression(node->left, vars)) {
            evaluate_helper(node->right, vars);
        }
    }
    else if (node->node_type == NODE_TYPE_OPERATOR || node->node_type == NODE_TYPE_VALUE || node->node_type == NODE_TYPE_ID || node->node_type == NODE_TYPE_RELOP)
        return;
}


void evaluate_node(node_t* node) {
    int vars[26];
    evaluate_helper(node, vars);
}

void print_prefix(node_t* node) {
    if (!node)
        return;

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
            printf("%c ", node->data.var_name);
            break;
        case NODE_TYPE_VALUE:
            printf("%d ", node->data.val);
            break;
        case NODE_TYPE_ASSIGN:
        case NODE_TYPE_OPERATOR:
            printf("%c ", node->data.op);
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
            printf("%s ", node->data.relop);
            break;
    }

    print_prefix(node->left);
    print_prefix(node->right);
}

void print_postfix(node_t* node) {
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
            printf("%c ", node->data.var_name);
            break;
        case NODE_TYPE_VALUE:
            printf("%d ", node->data.val);
            break;
        case NODE_TYPE_ASSIGN:
        case NODE_TYPE_OPERATOR:
            printf("%c ", node->data.op);
            break;
    }
}