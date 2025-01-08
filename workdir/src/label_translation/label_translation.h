
#ifndef LABEL_TRANSLATION_H
#define LABEL_TRANSLATION_H

#include <stdlib.h>
#include <stdio.h>

typedef struct label_node_s label_node_t;

struct label_node_s {
    int label;
    int address;
    label_node_t* next;
};

label_node_t* label_head_node;
label_node_t* label_tail_node;

void create_label_node(int label, int address);
int get_label_address(int label);


#endif