#include "type_table.h"



field_list_t* create_field_list_entry(char* name, type_table_t* struct_name, type_table_t* field_type) {
    field_list_t* entry = (field_list_t*) malloc(sizeof(field_list_t));
    entry->name = strdup(name);
    entry->type = field_type;
    entry->field_index = struct_name->num_fields;
    struct_name->num_fields++;

    if (struct_name->fields == NULL) {
        struct_name->fields = entry;
    }
    else {
        field_list_t* tail;
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


type_table_t* create_type_table_entry(char* name, int size) {
    type_table_t* entry = (type_table_t*) malloc(sizeof(type_table_t));

    entry->fields = NULL;
    entry->name = strdup(name);
    entry->next = NULL;
    entry->num_fields = 0;
    entry->size = size;

    type_table_t* tail;
    while(tail->next != NULL)
        tail = tail->next;
    tail->next = entry;
    
    return entry;
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

initialize_type_table() {
    type_table_t* int_type = create_type_table_entry("int", 1);
    type_table_t* str_type = create_type_table_entry("str", 1);
    type_table_t* bool_type = create_type_table_entry("bool", 1);
    type_table_t* void_type = create_type_table_entry("void", 0);
    type_table_t* arr_type = create_type_table_entry("arr", 1);
    type_table_t* ptr_type = create_type_table_entry("ptr", 1);
    type_table_t* unset_type = create_type_table_entry("unset", 0);

    default_types = (primitive_types_t*) malloc(sizeof(primitive_types_t));

    default_types->int_type = int_type;
    default_types->str_type = str_type;
    default_types->bool_type = bool_type;
    default_types->void_type = void_type;
    default_types->arr_type = arr_type;
    default_types->ptr_type = ptr_type;
    default_types->unset_type = unset_type;
}