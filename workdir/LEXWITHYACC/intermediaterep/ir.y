%{
#include <stdio.h>
#include <stdlib.h>
#include "./tree.h"


int yylex();
void yyerror(const char*);
%}

%token NUM
%left '+' '-'
%left '*' '/'

%union {
    Node* node;
}

%%

start   : expr '\n' { printf("Eval: %d\nParse complete\n", evaluate($<node>1)); exit(0); }
        ;

expr    : expr '+' expr     { addLeftSubTree($<node>2, $<node>1); addRightSubTree($<node>2, $<node>3); $<node>$ = $<node>2; }
        | expr '-' expr     { addLeftSubTree($<node>2, $<node>1); addRightSubTree($<node>2, $<node>3); $<node>$ = $<node>2; }
        | expr '*' expr     { addLeftSubTree($<node>2, $<node>1); addRightSubTree($<node>2, $<node>3); $<node>$ = $<node>2; }
        | expr '/' expr     { addLeftSubTree($<node>2, $<node>1); addRightSubTree($<node>2, $<node>3); $<node>$ = $<node>2; }
        | '(' expr ')'      { $<node>$ = $<node>2; }
        | NUM               { $<node>$ = $<node>1; }
        ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    yyparse();
    return 0;
}