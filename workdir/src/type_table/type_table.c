#include "type_table.h"

#include "../class_table/class_table.h"


field_list_t* create_field_list_entry(char* name, type_table_t* struct_name, type_table_t* field_type) {
    field_list_t* entry = (field_list_t*) malloc(sizeof(field_list_t));
    entry->name = strdup(name);
    entry->type = field_type;
    entry->field_index = struct_name->num_fields;
    struct_name->num_fields++;
    struct_name->size = struct_name->num_fields;


    if (struct_name->fields == NULL) {
        struct_name->fields = entry;
    }
    else {
        field_list_t* tail = struct_name->fields;
        while(tail->next != NULL)
            tail = tail->next;

        tail->next = entry;
    }

    return entry;
}

void destroy_field_list_entry(field_list_t* entry) {
    if (entry->next != NULL)
        destroy_field_list_entry(entry->next);
    free(entry->name);
    free(entry);
}


field_list_t* field_lookup(type_table_t* tuple, char* field_name) {
    if (tuple == NULL)
        return NULL;

    for (field_list_t* it = tuple->fields; it != NULL; it = it->next) {
        if (strcmp(it->name, field_name) == 0) {
            return it;
        }
    }

    return NULL;
}


type_table_t* create_type_table_entry(char* name, int size, decl_node_t* fields, var_type_t type) {
    type_table_t* entry = (type_table_t*) malloc(sizeof(type_table_t));

    entry->fields = NULL;
    entry->name = strdup(name);
    entry->next = NULL;
    entry->num_fields = 0;
    entry->size = size;
    entry->type = type;

    if (type_table == NULL) {
        type_table = entry;
    }
    else {
        type_table_t* tail = type_table;
        while(tail->next != NULL)
            tail = tail->next;
        tail->next = entry;

    }

    if (fields != NULL) {
        entry->size = 0;
        for (decl_node_t* it = fields; it != NULL; it = it->next) {
            create_field_list_entry(it->name, entry, it->type);
        }
    }
    
    return entry;
}

type_table_t* create_class_type_entry(class_table_t* details) {
    type_table_t* entry = (type_table_t*) malloc(sizeof(type_table_t));

    entry->fields = NULL;
    entry->name = strdup(details->name);
    entry->next = NULL;
    entry->size = 2;
    entry->type = VAR_TYPE_CLASS;
    entry->class_details = details;

    if (type_table == NULL) {
        type_table = entry;
    }
    else {
        type_table_t* tail = type_table;
        while(tail->next != NULL)
            tail = tail->next;
        tail->next = entry;

    }

    return entry;
}


type_table_t* create_user_type_entry(char* name, decl_node_t* fields) {
    type_table_t* entry = (type_table_t*) malloc(sizeof(type_table_t));

    entry->fields = NULL;
    entry->name = strdup(name);
    entry->next = NULL;
    entry->size = 0;
    entry->type = VAR_TYPE_CUSTOM;


    if (type_table == NULL) {
        type_table = entry;
    }
    else {
        type_table_t* tail = type_table;
        while(tail->next != NULL)
            tail = tail->next;
        tail->next = entry;
    }

    if (fields == NULL) {
        yyerror("{type_table:create_user_type_entry} Empty declaration for type");
        exit(1);
    }

    for (decl_node_t* field = fields; field != NULL; field = field->next) {
        type_table_t* field_type = get_type_table_entry(field->undeclared_type);
        if (field_type == NULL) {
            if (strcmp(field->undeclared_type, name) == 0) {
                field_type = entry;
            }
            else {
                yyerror("{type_table:create_user_type_entry} Undeclared type");
                exit(1);
            }
        }
        
        create_field_list_entry(field->name, entry, field_type);
    }

    if (entry->num_fields > 8) {
        yyerror("{type_table:create_user_type_entry} User defined types can't have more than 8 fields");
        exit(1);
    }
}

void destroy_type_table_entry(type_table_t* entry) {
    if (entry->next)
        destroy_type_table_entry(entry->next);
    
    free(entry->name);
    destroy_field_list_entry(entry->fields);
    free(entry);
}


type_table_t* get_type_table_entry(char* name) {
    for (type_table_t* entry = type_table; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) 
            return entry;
    }

    return NULL;
}

field_list_t* get_field_entry(type_table_t* struct_name, int field_index) {
    for (field_list_t* entry = struct_name->fields; entry != NULL; entry = entry->next) {
        if (entry->field_index == field_index)
            return entry;
    }

    return NULL;
}

bool is_primitive_type(type_table_t* type) {
    if (
        (type->type == VAR_TYPE_PRIMITIVE) &&
        type != default_types->unset_type
    ) {
        return true;
    }

    return false;
}

bool is_user_defined_type(type_table_t* type) {
    return (type->type == VAR_TYPE_CUSTOM);
}

bool is_tuple(type_table_t* type) {
    return (type->type == VAR_TYPE_TUPLE);
}

bool is_class(type_table_t* type) {
    return (type->type == VAR_TYPE_CLASS);
}


void initialize_type_table() {
    type_table = NULL;


    type_table_t* int_type = create_type_table_entry("int", 1, NULL, VAR_TYPE_PRIMITIVE);
    type_table_t* str_type = create_type_table_entry("str", 1, NULL, VAR_TYPE_PRIMITIVE);
    type_table_t* bool_type = create_type_table_entry("bool", 1, NULL, VAR_TYPE_PRIMITIVE);
    type_table_t* void_type = create_type_table_entry("void", 0, NULL, VAR_TYPE_PRIMITIVE);
    type_table_t* arr_type = create_type_table_entry("arr", 1, NULL, VAR_TYPE_COMPOUND);
    type_table_t* ptr_type = create_type_table_entry("ptr", 1, NULL, VAR_TYPE_COMPOUND);
    type_table_t* func_type = create_type_table_entry("func", 0, NULL, VAR_TYPE_COMPOUND);
    type_table_t* unset_type = create_type_table_entry("unset", 0, NULL, VAR_TYPE_PRIMITIVE);

    default_types = (primitive_types_t*) malloc(sizeof(primitive_types_t));

    default_types->int_type = int_type;
    default_types->str_type = str_type;
    default_types->bool_type = bool_type;
    default_types->void_type = void_type;
    default_types->arr_type = arr_type;
    default_types->ptr_type = ptr_type;
    default_types->unset_type = unset_type;
    default_types->func_type = func_type;
}