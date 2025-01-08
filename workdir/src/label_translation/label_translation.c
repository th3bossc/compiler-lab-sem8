#include "label_translation.h"


void create_label_node(int label, int address) {
    label_node_t* node = (label_node_t*) malloc(sizeof(label_node_t));

    node->label = label;
    node->address = address;
    node->next = NULL;

    if (label_head_node == NULL) {
        label_head_node = node;
        label_tail_node = node;
    }
    else {
        label_tail_node->next = node;
        label_tail_node = label_tail_node->next;
    }
}

int get_label_address(int label) {
    for (label_node_t* node = label_head_node; node != NULL; node = node->next) {
        if (node->label == label)
            return node->address;
    }
    perror("unknown label address");
    exit(1);
}