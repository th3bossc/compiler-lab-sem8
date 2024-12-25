#ifndef TREE_H
#define TREE_H

#include <stdlib.h>


typedef union Attribute {
    int nVal;
    char cVal;
} Attribute;

typedef struct Node {
    int isLeaf;
    Attribute val;
    struct Node* left;
    struct Node* right;
} Node;

Node* initializeNode(Attribute, int);
void addLeftSubTree(Node*, Node*);
void addRightSubTree(Node*, Node*);
int evaluate(Node*);


#endif