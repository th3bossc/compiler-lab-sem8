#include "activation_record.h"

activation_record_t* activation_stack;

void activation_stack_initialize() {
    activation_stack = NULL;
}

void activation_stack_destroy(activation_record_t* stack) {
    if (stack == NULL)
        return;

    activation_stack_destroy(stack->next);
    free(stack);
}

void activation_stack_push_func_entry(global_symbol_table_t* func_entry, local_symbol_table_t* l_symbol_table, int* num_used_regs) {
    printf("pushing %s into stack\n", func_entry->name);
    activation_record_t* entry = (activation_record_t*) malloc(sizeof(activation_record_t));
    entry->type = ACT_REC_TYPE_FUNCTION;
    entry->entry.func_entry = func_entry;
    entry->l_symbol_table = l_symbol_table;
    entry->num_used_regs = num_used_regs;

    entry->next = activation_stack;
    activation_stack = entry;
}

void activation_stack_push_method_entry(class_method_t* method_entry, local_symbol_table_t* l_symbol_table, int* num_used_regs) {
    activation_record_t* entry = (activation_record_t*) malloc(sizeof(activation_record_t));
    entry->type = ACT_REC_TYPE_CLASS_METHOD;
    entry->entry.method_entry = method_entry;
    entry->l_symbol_table = l_symbol_table;
    entry->num_used_regs = num_used_regs;

    entry->next = activation_stack;
    activation_stack = entry;
}

void activation_stack_pop() {
    if (activation_stack == NULL)
        return;
    printf("popping %s from stack\n", activation_stack->entry.func_entry->name);
    activation_stack = activation_stack->next;
}

activation_record_t* activation_stack_top() {
    return activation_stack;
}



