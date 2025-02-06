#include "activation_record.h"

function_metadata_t* current_block;
class_stack_t* class_stack;


void function_metadata_update_func(global_symbol_table_t* func_entry, local_symbol_table_t* l_symbol_table, int* num_used_regs) {
    function_metadata_t* entry = (function_metadata_t*) malloc(sizeof(function_metadata_t));

    entry->entry.func_entry = func_entry;
    entry->l_symbol_table = l_symbol_table;
    entry->num_used_regs = num_used_regs;
    entry->type = ACT_REC_TYPE_FUNCTION;

    current_block = entry;
}


void function_metadata_update_member_method(class_method_t* method_entry, local_symbol_table_t* l_symbol_table, int* num_used_regs) {
    function_metadata_t* entry = (function_metadata_t*) malloc(sizeof(function_metadata_t));

    entry->entry.method_entry = method_entry;
    entry->l_symbol_table = l_symbol_table;
    entry->num_used_regs = num_used_regs;
    entry->type = ACT_REC_TYPE_CLASS_METHOD;

    current_block = entry;
}

function_metadata_t* function_metdata_current() {
    return current_block;
}


void class_stack_initialize() {
    class_stack = NULL;
}

void class_stack_destroy(class_stack_t* stack) {
    if (stack == NULL)
        return;

    class_stack_destroy(stack->next);
    free(stack);
}

void class_stack_push(global_symbol_table_t* class_var) {
    class_stack_t* entry = (class_stack_t*) malloc(sizeof(class_stack_t));

    entry->class_var = class_var;
    entry->next = class_stack;
    class_stack = entry;
}

void class_stack_pop() {
    if (class_stack == NULL)
        return;

    class_stack = class_stack->next;
}

class_stack_t* class_stack_top() {
    return class_stack;
}


