#include "./tree.h"

Node* initializeNode(Attribute val, int leaf) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->val = val;
    node->isLeaf = leaf;
    node->left = NULL;
    node->right = NULL;

    return node;
}



void addLeftSubTree(Node* parent, Node* left) {
    parent->left = left;
}

void addRightSubTree(Node* parent, Node* right) {
    parent->right = right;
}


int evaluate(Node* root) {
    if (!root)
        return 0;
    if (root->isLeaf)
        return root->val.nVal;
    
    int lVal = evaluate(root->left);
    int rVal = evaluate(root->right);

    int answer = 0;
    switch (root->val.cVal) {
        case '+':
            answer = lVal + rVal;
            break;
        case '-':
            answer = lVal - rVal;
            break;
        case '*':
            answer = lVal * rVal;
            break;
        case '/':
            answer = lVal / rVal;
            break;
    }


    return answer;
}

