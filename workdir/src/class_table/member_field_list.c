#include "member_field_list.h"
#include "class_table.h"

class_field_t* create_class_field_entry(char* name, type_table_t* type) {
    class_field_t* entry = (class_field_t*) malloc(sizeof(class_field_t));
    entry->name = strdup(name);
    entry->type = type;
    entry->next = NULL;
    
    return entry;
}

void destroy_class_field_entry(class_field_t* entry) {
    if (entry == NULL)
        return;

    destroy_class_field_entry(entry->next);
    free(entry);
}


class_field_t* class_field_lookup(class_table_t* class_name, char* field_name) {
    for (class_field_t* field = class_name->fields; field != NULL; field = field->next) {
        if (strcmp(field->name, field_name) == 0) {
            return field;
        }
    }

    return NULL;
}
