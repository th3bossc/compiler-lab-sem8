#include "member_func_list.h"
#include "class_table.h"

class_method_t* create_class_method_entry(class_table_t* class_details, char* name, type_table_t* return_type, decl_node_t* params, ast_node_t* func_body, decl_node_t* local_decls, label_index_t label, int method_index) {
    class_method_t* entry = (class_method_t*) malloc(sizeof(class_method_t));

    entry->name = strdup(name);
    entry->return_type = return_type;
    entry->local_decls = local_decls;
    entry->func_body = func_body;
    entry->label = label;
    entry->params = params;
    entry->class = class_details;
    entry->method_index = method_index;
    entry->next = NULL;

    return entry;
}


void destroy_class_method_entry(class_method_t* entry) {
    if (entry == NULL)
        return;

    free(entry->name);
    free(entry->func_body);
    free(entry->params);

    destroy_class_method_entry(entry->next);
    free(entry);
}


class_method_t* class_method_lookup(class_table_t* class_name, char* method_name, int num_args) {

    for (class_method_t* method = class_name->methods; method != NULL; method = method->next) {
        if (strcmp(method->name, method_name) == 0) {
            if (num_args == -1 || count_num_params(method->params) == num_args)
                return method;
        }
    }

    return NULL;
}