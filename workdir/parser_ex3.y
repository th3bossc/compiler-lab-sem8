%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./utils/node.h"
#include "./utils/code_generation.h"
void yyerror(const char*);
int yylex();
void generate_code(FILE*, node_t*);
FILE* fp = NULL;
%}

%token NUM

%left '+' '-'
%left '*' '/'

%union {
    node_t* node;
}

%%

start   : expr '\n'     { printf("\nParse Complete"); printf("\nPrefix: "); print_prefix($<node>1); printf("\nPostfix: "); print_postfix($<node>1); printf("\n"); generate_code(fp, $<node>1); destroy_node($<node>1); exit(0); }
        ;

// expr    : expr '+' expr { append_left_child($<node>2, $<node>1); append_right_child($<node>2, $<node>3); $<node>$ = $<node>2; }
//         | expr '-' expr { append_left_child($<node>2, $<node>1); append_right_child($<node>2, $<node>3); $<node>$ = $<node>2; }
//         | expr '*' expr { append_left_child($<node>2, $<node>1); append_right_child($<node>2, $<node>3); $<node>$ = $<node>2; }
//         | expr '/' expr { append_left_child($<node>2, $<node>1); append_right_child($<node>2, $<node>3); $<node>$ = $<node>2; }
//         | '(' expr ')'  { $<node>$ = $<node>2; }
//         | NUM           { $<node>$ = $<node>1; }
//         ;

expr    : '+' expr expr { append_left_child($<node>1, $<node>2); append_right_child($<node>1, $<node>3); $<node>$ = $<node>1; }
        | '-' expr expr { append_left_child($<node>1, $<node>2); append_right_child($<node>1, $<node>3); $<node>$ = $<node>1; }
        | '*' expr expr { append_left_child($<node>1, $<node>2); append_right_child($<node>1, $<node>3); $<node>$ = $<node>1; }
        | '/' expr expr { append_left_child($<node>1, $<node>2); append_right_child($<node>1, $<node>3); $<node>$ = $<node>1; }
        | NUM           { $<node>$ = $<node>1; }
        ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "\nError: %s\n", s);
} 

void generate_code(FILE* fp, node_t* node) {
    generate_headers(fp);
    reg_index_t data = generate_arithmetic_code(node, fp);
    reg_index_t ret_val = print_content(data, fp);
    exit_program(fp);
    free_register(data);
    free_register(ret_val);

}


int main(int argc, char* argv[]) {
    if (argc > 1)
        fp = fopen(argv[1], "w");
    else 
        fp = fopen("output.xsm", "w");
    reset_registers();


    yyparse();
    return 0;
}