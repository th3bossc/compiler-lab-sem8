%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "src/code_generation/code_generation.h"
#include "src/symbol_table/global_symbol_table.h"
#include "src/type_table/type_table.h"
#include "src/node/args_node.h"

extern FILE *yyin;
void yyerror(const char*);
int yylex();
void generate_code(ast_node_t*, FILE*);
FILE* output_path = NULL;
bool evaluate_mode = false;
void parser_complete_handler(ast_node_t*, FILE*);

type_table_t* var_type;
type_table_t* arg_type;
type_table_t* return_type;

// #define YYDEBUG 1
// int yydebug = 1;
%}

%token SEMICOLON COMMA
%token END_BLOCK BEGIN_BLOCK
%token BEGIN_DECL END_DECL
%token NUM ID 
%token INT STR
%token WRITE READ
%token IF THEN ELSE ENDIF 
%token WHILE DO ENDWHILE
%token REPEAT UNTIL ENDREPEAT
%token BREAK CONTINUE
%token GT LT GTE LTE EQUALS NOT_EQUALS
%token STRING_LITERAL
%token MAIN

%left GT LT GTE LTE
%left EQUALS NOT_EQUALS

%left '+' '-'
%left '*' '/'
%left '&'
%nonassoc '[' ']' '%'



%union {
    ast_node_t* node;
    decl_node_t* decl_node;
    args_node_t* args_node;
    char c_val;
    char* s_val;
    int n_val;
}

%%

// start       : BEGIN_DECL decl_list END_DECL BEGIN_BLOCK stmt_list END_BLOCK     { printf("Parse Complete\nPrefix: "); print_prefix($<node>5); printf("\n"); print_symbol_table(); parser_complete_handler($<node>5, output_path); exit(0); }
//             | BEGIN_DECL decl_list END_DECL stmt_list                           { printf("Parse Complete\nPrefix: "); print_prefix($<node>4); printf("\n"); print_symbol_table(); parser_complete_handler($<node>4, output_path); exit(0); }
//             | BEGIN_BLOCK stmt_list END_BLOCK                                   { printf("Parser Complete\nPrefix: "); print_prefix($<node>2); printf("\n"); print_symbol_table(); parser_complete_handler($<node>2, output_path); exit(0); }
//             | BEGIN_DECL decl_list END_DECL                                     { printf("Parser Complete\n"); print_symbol_table(); exit(0); }
//             ;

program     : global_decl_block func_def_block  { printf("Parse complete [p1]\n"); print_prefix($<node>2); printf("\nGlobal Symbol Table: "); print_symbol_table(); parser_complete_handler($<node>2, output_path); }
            | global_decl_block                 { printf("Parse complete [p2]\n"); printf("\nGlobal Symbol Table: "); print_symbol_table(); }
            | func_def_block                    { printf("Parse complete [p3]\n"); print_prefix($<node>1); parser_complete_handler($<node>1, output_path); }
            ;


global_decl_block   : BEGIN_DECL global_decls_list END_DECL
                    | BEGIN_DECL END_DECL
                    ;

global_decls_list   : global_decls_list global_decls
                    | global_decls
                    ;

global_decls    : type gloabl_id_list SEMICOLON
                ;

gloabl_id_list  : gloabl_id_list COMMA global_id 
                | global_id 
                ;

global_id   : ID                                        { create_global_symbol_table_entry($<s_val>1, var_type, 1); }
            | ID '[' NUM ']'                            { create_global_symbol_table_array_entry($<s_val>1, var_type, $<n_val>3, 1); }
            | ID '[' NUM ']' '[' NUM ']'                { create_global_symbol_table_array_entry($<s_val>1, var_type, $<n_val>3, $<n_val>6); }
            | '*' ID                                    { create_global_symbol_table_pointer_entry($<s_val>2, var_type, 1); }
            | ID '(' func_params_list ')'               { create_global_symbol_table_func_entry($<s_val>2, var_type, $<decl_node>4); }
            | ID '(' ')'                                { create_global_symbol_table_func_entry($<s_val>2, var_type, NULL); }
            ;

local_decl_block    : BEGIN_DECL local_decls_list END_DECL      { $<decl_node>$ = $<decl_node>2; }
                    | BEGIN_DECL END_DECL                       { $<decl_node>$ = NULL; }
                    ;

local_decls_list    : local_decls_list local_decls      { $<decl_node>$ = join_decl_nodes($<decl_node>1, $<decl_node>2); }
                    | local_decls                       { $<decl_node>$ = $<decl_node>1; }
                    ;


local_decls     : type local_id_list SEMICOLON          { $<decl_node>$ = $<decl_node>2; }
                ;


local_id_list   : local_id_list COMMA ID    { $<decl_node>$ = join_decl_nodes($<decl_node>1, create_decl_node($<s_val>3, var_type)); }
                | ID                        { $<decl_node>$ = create_decl_node($<s_val>1, var_type); }

  
type        : INT       { var_type = default_types->int_type; }
            | STR       { var_type = default_types->str_type; }
            ;


ret_type    : INT       { return_type = default_types->int_type; }
            | STR       { return_type = default_types->str_type; }
            ;

// decl_list   : decl_list decl        
//             | decl
//             ;

// decl        : type id_list SEMICOLON 
//             ;

// id_list     : id_list COMMA id_type 
//             | id_type
//             ;

// id_type     : ID                            { create_symbol_table_entry($<s_val>1, var_type, 1); }
//             | ID '[' NUM ']'                { create_symbol_table_array_entry($<s_val>1, var_type, $<n_val>3, 1); }
//             | ID '[' NUM ']' '[' NUM ']'    { create_symbol_table_array_entry($<s_val>1, var_type, $<n_val>3, $<n_val>6); }
//             | '*' ID                        { create_symbol_table_pointer_entry($<s_val>2, var_type, 1); }
//             ; 


//////////////////////////////////////////////////////////////////////////////////////////////////////

func_def_block  : func_def_block func_def       { $<node>$ = create_program_node($<node>1, $<node>2); }
                | func_def                      { $<node>$ = $<node>1; }
                ;


func_def    : ret_type ID '(' func_params_list ')' '{' local_decl_block func_body '}'       { $<node>$ = create_func_body_node($<s_val>2, return_type, $<decl_node>4, $<decl_node>7, $<node>8); }
            | ret_type ID '(' func_params_list ')' '{' func_body '}'                        { $<node>$ = create_func_body_node($<s_val>2, return_type, $<decl_node>4, NULL, $<node>7); }
            | ret_type ID '(' ')' '{' local_decl_block func_body '}'                        { $<node>$ = create_func_body_node($<s_val>2, return_type, NULL, $<decl_node>6, $<node>7); }
            | ret_type ID '(' ')' '{' func_body '}'                                         { $<node>$ = create_func_body_node($<s_val>2, return_type, NULL, NULL, $<node>6); }
            ; 

func_params_list    : func_params_list COMMA func_param     { $<decl_node>$ = join_decl_nodes($<decl_node>1, $<decl_node>3); }
                    | func_param                            { $<decl_node>$ = $<decl_node>1; }
                    ;

func_param      : arg_type ID   { $<decl_node>$ = create_decl_node($<s_val>2, arg_type); }
                ;

arg_type    : INT       { arg_type = default_types->int_type; }
            | STR       { arg_type = default_types->str_type; }
            ;

func_body   : BEGIN_BLOCK stmt_list END_BLOCK       { $<node>$ = $<node>2; }
            ;

//////////////////////////////////////////////////////////////////////////////////////////////////////

stmt_list   : stmt_list stmt                    { $<node>$ = create_connector_node($<node>1, $<node>2); }
            | stmt                              { $<node>$ = $<node>1; }
            ;

stmt        : stmt_body SEMICOLON               { $<node>$ = $<node>1; }
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

stmt_assign : ID '=' expr                               { $<node>$ = create_assignment_node($<s_val>1, $<node>3); }
            | ID '[' expr ']' '=' expr                  { $<node>$ = create_arr_assignment_node($<s_val>1, $<node>3, $<node>6); }
            | ID '[' expr ']' '[' expr ']' '=' expr     { $<node>$ = create_2d_arr_assignment_node($<s_val>1, $<node>3, $<node>6, $<node>9); }
            | '*' ID '=' expr                           { $<node>$ = create_ptr_assignment_node($<s_val>2, $<node>4); }
            ;

stmt_write  : WRITE '(' expr ')'                { $<node>$ = create_write_node($<node>3); }
            ;

stmt_read   : READ '(' expr ')'                 { $<node>$ = create_read_node($<node>3); }
            ;


stmt_if     : IF '(' expr ')' THEN stmt_list ELSE stmt_list ENDIF  { $<node>$ = create_ifelse_node($<node>3, $<node>6, $<node>8); }
            | IF '(' expr ')' THEN stmt_list ENDIF                 { $<node>$ = create_ifelse_node($<node>3, $<node>6, NULL); }
            ;

stmt_while  : WHILE '(' expr ')' DO stmt_list ENDWHILE   { $<node>$ = create_while_node($<node>3, $<node>6); }
            ;

stmt_do_while   : DO stmt_list WHILE '(' expr ')' ENDWHILE          { $<node>$ = create_do_while_node($<node>5, $<node>2); }
                ;

stmt_repeat_until   : REPEAT stmt_list UNTIL '(' expr ')' ENDREPEAT { $<node>$ = create_repeat_node($<node>5, $<node>2); }


expr        : expr '+' expr                     { $<node>$ = create_operator_node($<c_val>2, $<node>1, $<node>3); }
            | expr '-' expr                     { $<node>$ = create_operator_node($<c_val>2, $<node>1, $<node>3); }
            | expr '*' expr                     { $<node>$ = create_operator_node($<c_val>2, $<node>1, $<node>3); }
            | expr '/' expr                     { $<node>$ = create_operator_node($<c_val>2, $<node>1, $<node>3); }
            | expr '%' expr                     { $<node>$ = create_operator_node($<c_val>2, $<node>1, $<node>3); }
            | '(' expr ')'                      { $<node>$ = $<node>2; }
            | NUM                               { $<node>$ = create_num_node($<n_val>1); }
            | ID                                { $<node>$ = create_id_node($<s_val>1); }
            | expr GT expr                      { $<node>$ = create_relop_node(">", $<node>1, $<node>3); }
            | expr LT expr                      { $<node>$ = create_relop_node("<", $<node>1, $<node>3); }
            | expr EQUALS expr                  { $<node>$ = create_relop_node("==", $<node>1, $<node>3); }
            | expr NOT_EQUALS expr              { $<node>$ = create_relop_node("!=", $<node>1, $<node>3); }
            | expr GTE expr                     { $<node>$ = create_relop_node(">=", $<node>1, $<node>3); }
            | expr LTE expr                     { $<node>$ = create_relop_node("<=", $<node>1, $<node>3); }
            | STRING_LITERAL                    { $<node>$ = create_string_node($<s_val>1); }
            | '*' expr                          { $<node>$ = create_ptr_ref_node($<node>2); }
            | '&' expr                          { $<node>$ = create_ptr_deref_node($<node>2); }
            | ID '[' expr ']'                   { $<node>$ = create_arr_index_node($<s_val>1, $<node>3); }
            | ID '[' expr ']' '[' expr ']'      { $<node>$ = create_2d_arr_index_node($<s_val>1, $<node>3, $<node>6); }
            | ID '(' args_list ')'              { $<node>$ = create_func_call_node($<s_val>1, $<args_node>3); }
            | ID '(' ')'                        { $<node>$ = create_func_call_node($<s_val>1, NULL); }
            ;

args_list   : args_list COMMA expr              { $<args_node>$ = join_args_nodes($<args_node>1, create_args_node($<node>3)); }
            | expr                              { $<args_node>$ = create_args_node($<node>1); }
            ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "\nError: %s\n", s);
} 

void parser_complete_handler(ast_node_t* node, FILE* fp) {
    if (evaluate_mode) {
        // evaluate_node(node);
    }
    else {
        generate_program(node, fp);
    }
}


int main(int argc, char* argv[]) {
    initialize_type_table();
    create_global_table();
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0 && (i+1) < argc) {
            FILE* fp = fopen(argv[i+1], "r");
            if (fp) {
                yyin = fp;
                printf("input: %p\n", fp);
            }
        }
        else if (strcmp(argv[i], "--output") == 0 && (i+1) < argc) {
            FILE* fp = fopen(argv[i+1], "w");
            if (fp) {
                output_path = fp;
                printf("output: %p\n", fp);
            }
        }
        else if (strcmp(argv[i], "--evaluate") == 0) {
            evaluate_mode = true;
        }
    }

    yyparse();
    return 0;
}