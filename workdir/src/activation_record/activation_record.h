#ifndef ACTIVATION_RECORD_H
#define ACTIVATION_RECORD_H

#include <stdlib.h>
#include "../symbol_table/global_symbol_table.h"
#include "../class_table/class_table.h"

typedef enum activation_record_type_s activation_record_type_t;
typedef struct activation_record_s activation_record_t;
typedef union function_details_s function_details_t;
typedef struct class_stack_s class_stack_t;

enum activation_record_type_s {
    ACT_REC_TYPE_FUNCTION,
    ACT_REC_TYPE_CLASS_METHOD,
};

union function_details_s {
    global_symbol_table_t* func_entry;
    class_method_t* method_entry;
};

struct activation_record_s {
    activation_record_type_t type;
    function_details_t entry;

    local_symbol_table_t* l_symbol_table;
    int* num_used_regs;

    activation_record_t* next;
};

struct class_stack_s {
    global_symbol_table_t* class_var;
    class_stack_t* next;
};



void activation_stack_push_func_entry(global_symbol_table_t* func_entry, local_symbol_table_t* l_symbol_table, int* num_used_regs);
void activation_stack_push_method_entry(class_method_t* method_entry, local_symbol_table_t* l_symbol_table, int* num_used_regs);
void activation_stack_pop();
activation_record_t* activation_stack_top();

void activation_stack_initialize();
void activation_stack_destroy(activation_record_t* stack);


void class_stack_push(global_symbol_table_t* class_var);
void class_stack_pop();
class_stack_t* class_stack_top();

void class_stack_initialize();
void class_stack_destroy(class_stack_t* stack);

#endif