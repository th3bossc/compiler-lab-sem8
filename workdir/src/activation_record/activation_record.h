#ifndef ACTIVATION_RECORD_H
#define ACTIVATION_RECORD_H

#include <stdlib.h>
#include "../symbol_table/global_symbol_table.h"
#include "../class_table/class_table.h"

typedef enum function_type_s function_type_t;
typedef struct function_metadata_s function_metadata_t;
typedef union function_details_s function_details_t;
typedef struct class_stack_s class_stack_t;

enum function_type_s {
    ACT_REC_TYPE_FUNCTION,
    ACT_REC_TYPE_CLASS_METHOD,
};

union function_details_s {
    global_symbol_table_t* func_entry;
    class_method_t* method_entry;
};

struct function_metadata_s {
    function_type_t type;
    function_details_t entry;

    local_symbol_table_t* l_symbol_table;
    int* num_used_regs;
};

struct class_stack_s {
    global_symbol_table_t* class_var;
    class_stack_t* next;
};



void function_metadata_update_func(global_symbol_table_t* func_entry, local_symbol_table_t* l_symbol_table, int* num_used_regs);
void function_metadata_update_member_method(class_method_t* method_entry, local_symbol_table_t* l_symbol_table, int* num_used_regs);
function_metadata_t* function_metdata_current();


void class_stack_push(global_symbol_table_t* class_var);
void class_stack_pop();
class_stack_t* class_stack_top();

void class_stack_initialize();
void class_stack_destroy(class_stack_t* stack);

#endif