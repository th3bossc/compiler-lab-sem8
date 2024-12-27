%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./utils/node.h"

void yyerror(const char*);
int yylex();
FILE* fp = NULL;
%}

%token NUM

%left '+' '-'
%left '*' '/'

%union {
    node_t* node;
}

%%

start   : expr '\n'     { printf("\nParse Complete"); printf("\nPrefix: "); print_prefix($<node>1); printf("\nPostfix: "); print_postfix($<node>1); printf("\n"); destroy_node($<node>1); exit(0); }
        ;

expr    : expr '+' expr { append_left_child($<node>2, $<node>1); append_right_child($<node>2, $<node>3); $<node>$ = $<node>2; }
        | expr '-' expr { append_left_child($<node>2, $<node>1); append_right_child($<node>2, $<node>3); $<node>$ = $<node>2; }
        | expr '*' expr { append_left_child($<node>2, $<node>1); append_right_child($<node>2, $<node>3); $<node>$ = $<node>2; }
        | expr '/' expr { append_left_child($<node>2, $<node>1); append_right_child($<node>2, $<node>3); $<node>$ = $<node>2; }
        | '(' expr ')'  { $<node>$ = $<node>2; }
        | NUM           { $<node>$ = $<node>1; }
        ;


%%

void yyerror(const char* s) {
    fprintf(stderr, "\nError: %s\n", s);
} 


int main(int argc, char argv[]) {
    yyparse();
    return 0;
}