#include "class_table.h"
void append_to_class_table(class_table_t* entry);

void initialize_class_table() {
    class_table = NULL;
}


class_table_t* create_class_table_entry(char* name, class_decl_node_t* decl_list, class_decl_node_t* method_list, class_table_t* parent_class) {
    class_table_t* entry = (class_table_t*) malloc(sizeof(class_table_t));

    entry->name = strdup(name);
    entry->fields = NULL;
    entry->methods = NULL;
    entry->parent_class = parent_class;
    entry->num_fields = 0;
    entry->num_methods = 0;
    entry->next = NULL;
    append_to_class_table(entry);
    create_class_type_entry(entry);

    class_field_t* field_tail = NULL;
    class_method_t* method_tail = NULL;
    for (class_decl_node_t* node = decl_list; node != NULL; node = node->next) {
        if (node->node_type == CLASS_DECL_NODE_TYPE_FIELD) {
            type_table_t* field_type = get_type_table_entry(node->undeclared_type);
            if (field_type == NULL) {
                yyerror("{class_table:create_class_table_entry} Field type doesn't exist");
                exit(1);
            }
            class_field_t* field = create_class_field_entry(node->name, field_type, entry->num_fields);

            if (field_tail == NULL) {
                field_tail = field;
                entry->fields = field_tail;
            }
            else {
                field_tail->next = field;
                field_tail = field;
            }
            entry->num_fields++;
        }
        else {
            type_table_t* return_type = get_type_table_entry(node->undeclared_type);
            if (return_type == NULL) {
                yyerror("{class_table:create_class_table_entry} Field type doesn't exist");
                exit(1);
            }
            class_method_t* method = create_class_method_entry(entry, node->name, return_type, node->params, NULL, NULL, get_label());

            if (method_tail == NULL) {
                method_tail = method;
                entry->methods = method_tail;
            }
            else {
                method_tail->next = method;
                method_tail = method;
            }
            entry->num_methods++;
        }
    }

    for (class_decl_node_t* node = method_list; node != NULL; node = node->next) {
        class_method_t* target_method = class_method_lookup(entry, node->name);
        if (target_method == NULL) {
            yyerror("{class_table:create_class_table_entry} Method doesn't exist on class");
            exit(1);
        }

        type_table_t* return_type = get_type_table_entry(node->undeclared_type);
            if (return_type == NULL) {
                yyerror("{class_table:create_class_table_entry} Field type doesn't exist");
                exit(1);
            }
        if (return_type != target_method->return_type) {
            yyerror("{class_table:create_class_table_entry} Return type of method doesn't match the declaration");
            exit(1);
        }

        if (!verify_params_list(node->params, target_method->params)) {
            yyerror("{class_table:create_class_table_entry} Method parameters don't match the declaration");
            exit(1);
        }


        target_method->local_decls = node->local_decls;
        target_method->func_body = node->func_body;
    }

    return entry;
}

void destroy_class_table_entry(class_table_t* entry) {
    if (entry == NULL)
        return;

    free(entry->name);
    free(entry->fields);
    free(entry->methods);
    
    destroy_class_table_entry(entry->next);

    free(entry);
}


void append_to_class_table(class_table_t* entry) {
    if (class_table == NULL) {
        class_table = entry;
        return;
    }
    class_table_t* tail = class_table;
    while(tail->next != NULL)
        tail = tail->next;

    tail->next = entry;
}


class_table_t* class_table_lookup(char* name) {
    for (class_table_t* entry = class_table; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
    }

    return NULL;
}