%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./utils/node.h"
#include "./utils/code_generation.h"
extern FILE *yyin;
void yyerror(const char*);
int yylex();
void generate_code(node_t*, FILE*);
FILE* fp = NULL;
%}

%token NUM ID BEGIN_BLOCK END_BLOCK WRITE READ PUNCTUATION

%left '+' '-'
%left '*' '/'

%union {
    node_t* node;
    char op;
    char var_name;
    int val;
}

%%

start       : BEGIN_BLOCK stmt_list END_BLOCK   { printf("Parse Complete\nPrefix: "); print_prefix($<node>2); printf("\n"); generate_code($<node>2, fp); exit(0); }
            ;

stmt_list   : stmt_list stmt                    { $<node>$ = create_connector_node($<node>1, $<node>2); }
            | stmt                              { $<node>$ = $<node>1; }
            ;

stmt        : stmt_body PUNCTUATION
            ;

stmt_body   : stmt_assign                       { $<node>$ = $<node>1; }
            | stmt_read                         { $<node>$ = $<node>1; }
            | stmt_write                        { $<node>$ = $<node>1; }
            ;

stmt_assign : ID '=' expr                       { $<node>$ = create_assignment_node($<var_name>1, $<node>3); }
            ;

stmt_write  : WRITE '(' expr ')'                { $<node>$ = create_write_node($<node>3); }
            ;

stmt_read   : READ '(' ID ')'                   { $<node>$ = create_read_node($<var_name>3); }
            ;


expr        : expr '+' expr                     { $<node>$ = create_operator_node($<op>2, $<node>1, $<node>3); }
            | expr '-' expr                     { $<node>$ = create_operator_node($<op>2, $<node>1, $<node>3); }
            | expr '*' expr                     { $<node>$ = create_operator_node($<op>2, $<node>1, $<node>3); }
            | expr '/' expr                     { $<node>$ = create_operator_node($<op>2, $<node>1, $<node>3); }
            | '(' expr ')'                      { $<node>$ = $<node>2; }
            | NUM                               { $<node>$ = create_num_node($<val>1); }
            | ID                                { $<node>$ = create_id_node($<var_name>1); }
            ;


%%

void yyerror(const char* s) {
    fprintf(stderr, "\nError: %s\n", s);
} 

void generate_code(node_t* node, FILE* fp) {
    generate_headers(fp);
    fprintf(fp, "MOV SP, 4122\n");
    generate_statement_structure(node, fp);
    exit_program(fp);

}


int main(int argc, char* argv[]) {
    if (argc > 2) {
        FILE* input_fp = fopen(argv[1], "r");
        fp = fopen(argv[2], "w");
        yyin = input_fp;
    }
    else 
        fp = fopen("output.xsm", "w");
    reset_registers();


    yyparse();
    return 0;
}