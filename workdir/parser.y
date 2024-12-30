%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "core/code_generation.h"
extern FILE *yyin;
void yyerror(const char*);
int yylex();
void generate_code(node_t*, FILE*);
FILE* output_path = NULL;
bool evaluate_mode = false;
void parser_complete_handler(node_t*, FILE*);
%}

%token END_BLOCK BEGIN_BLOCK
%token NUM ID 
%token WRITE READ PUNCTUATION
%token IF THEN ELSE ENDIF 
%token WHILE DO ENDWHILE
%token REPEAT UNTIL ENDREPEAT
%token BREAK CONTINUE
%token GT LT GTE LTE EQUALS NOT_EQUALS

%left GT LT GTE LTE
%left EQUALS NOT_EQUALS

%left '+' '-'
%left '*' '/'

%union {
    node_t* node;
    char op;
    char var_name;
    int val;
}

%%

start       : BEGIN_BLOCK stmt_list END_BLOCK   { printf("Parse Complete\nPrefix: "); print_prefix($<node>2); printf("\n"); parser_complete_handler($<node>2, output_path); exit(0); }
            ;

stmt_list   : stmt_list stmt                    { $<node>$ = create_connector_node($<node>1, $<node>2); }
            | stmt                              { $<node>$ = $<node>1; }
            ;

stmt        : stmt_body PUNCTUATION             { $<node>$ = $<node>1; }
            ;

stmt_body   : stmt_assign                       { $<node>$ = $<node>1; }
            | stmt_read                         { $<node>$ = $<node>1; }
            | stmt_write                        { $<node>$ = $<node>1; }
            | stmt_if                           { $<node>$ = $<node>1; }
            | stmt_while                        { $<node>$ = $<node>1; }
            | stmt_do_while                     { $<node>$ = $<node>1; }
            | stmt_repeat_until                 { $<node>$ = $<node>1; }
            | BREAK                             { $<node>$ = create_break_node(); }
            | CONTINUE                          { $<node>$ = create_continue_node(); }
            ;

stmt_assign : ID '=' expr                       { $<node>$ = create_assignment_node($<var_name>1, $<node>3); }
            ;

stmt_write  : WRITE '(' expr ')'                { $<node>$ = create_write_node($<node>3); }
            ;

stmt_read   : READ '(' ID ')'                   { $<node>$ = create_read_node($<var_name>3); }
            ;


stmt_if     : IF '(' expr ')' THEN stmt_list ELSE stmt_list ENDIF  { $<node>$ = create_ifelse_node($<node>3, $<node>6, $<node>8); }
            | IF '(' expr ')' THEN stmt_list ENDIF                 { $<node>$ = create_ifelse_node($<node>3, $<node>6, NULL); }
            ;

stmt_while  : WHILE '(' expr ')' DO stmt_list ENDWHILE   { $<node>$ = create_while_node($<node>3, $<node>6); }
            ;

stmt_do_while   : DO stmt_list WHILE '(' expr ')' ENDWHILE          { $<node>$ = create_do_while_node($<node>5, $<node>2); }
                ;

stmt_repeat_until   : REPEAT stmt_list UNTIL '(' expr ')' ENDREPEAT { $<node>$ = create_repeat_node($<node>5, $<node>2); }


expr        : expr '+' expr                     { $<node>$ = create_operator_node($<op>2, $<node>1, $<node>3); }
            | expr '-' expr                     { $<node>$ = create_operator_node($<op>2, $<node>1, $<node>3); }
            | expr '*' expr                     { $<node>$ = create_operator_node($<op>2, $<node>1, $<node>3); }
            | expr '/' expr                     { $<node>$ = create_operator_node($<op>2, $<node>1, $<node>3); }
            | '(' expr ')'                      { $<node>$ = $<node>2; }
            | NUM                               { $<node>$ = create_num_node($<val>1); }
            | ID                                { $<node>$ = create_id_node($<var_name>1); }
            | expr GT expr                      { $<node>$ = create_relop_node(">", $<node>1, $<node>3); }
            | expr LT expr                      { $<node>$ = create_relop_node("<", $<node>1, $<node>3); }
            | expr EQUALS expr                  { $<node>$ = create_relop_node("==", $<node>1, $<node>3); }
            | expr NOT_EQUALS expr              { $<node>$ = create_relop_node("!=", $<node>1, $<node>3); }
            | expr GTE expr                     { $<node>$ = create_relop_node(">=", $<node>1, $<node>3); }
            | expr LTE expr                     { $<node>$ = create_relop_node("<=", $<node>1, $<node>3); }
            ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "\nError: %s\n", s);
} 

void parser_complete_handler(node_t* node, FILE* fp) {
    if (evaluate_mode) {
        evaluate_node(node);
    }
    else {
        generate_program(node, fp);
    }
}


int main(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0 && (i+1) < argc) {
            FILE* fp = fopen(argv[i+1], "r");
            if (fp) {
                yyin = fp;
            }
        }
        else if (strcmp(argv[i], "--output") == 0 && (i+1) < argc) {
            FILE* fp = fopen(argv[i+1], "w");
            if (fp) {
                output_path = fp;
            }
        }
        else if (strcmp(argv[i], "--evaluate") == 0) {
            evaluate_mode = true;
        }
    }

    yyparse();
    return 0;
}