%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "src/code_generation/code_generation.h"
#include "src/symbol_table/global_symbol_table.h"
#include "src/type_table/type_table.h"
#include "src/node/args_node.h"
#include "src/instr_set/instr_set.h"
#include "src/class_table/class_table.h"
#include "src/node/class_decl_node.h"



extern FILE *yyin;
void yyerror(const char*);
int yylex();
FILE* output_path = NULL;
bool evaluate_mode = false;
void parser_complete_handler(ast_node_t*);

type_table_t* var_type_entry;
type_table_t* arg_type_entry;
type_table_t* return_type_entry;
type_table_t* tuple_field_type_entry;
type_table_t* class_field_type_entry;
int num_fields;

// #define YYDEBUG 1
// int yydebug = 1;
%}

%token SEMICOLON COMMA PERIOD
%token END_BLOCK BEGIN_BLOCK
%token BEGIN_DECL END_DECL
%token BEGIN_TYPE END_TYPE
%token BEGIN_CLASS END_CLASS
%token NUM ID 
%token INT STR
%token WRITE READ INIT_HEAP ALLOC FREE
%token IF THEN ELSE ENDIF 
%token WHILE DO ENDWHILE
%token REPEAT UNTIL ENDREPEAT
%token BREAK CONTINUE
%token GT LT GTE LTE EQUALS NOT_EQUALS AND OR
%token STRING_LITERAL
%token RETURN
%token TUPLE
%token NULLPTR
%token NEW
%token EXTENDS

%left OR
%left AND
%left GT LT GTE LTE
%left EQUALS NOT_EQUALS

%left '+' '-'
%left '*' '/'
%left '&'
%nonassoc '[' ']' '%'
%nonassoc PERIOD

%union {
    ast_node_t* node;
    decl_node_t* decl_node;
    args_node_t* args_node;
    class_decl_node_t* class_decl_node;
    type_table_t* type_table;
    char c_val;
    char* s_val;
    int n_val;
}

%%


program     : type_decl_block class_decl_block global_decl_block func_def_block  { printf("Parse complete [p1]\n"); print_prefix($<node>4); printf("\nGlobal Symbol Table: "); print_symbol_table(); parser_complete_handler($<node>4); }
            ;

type_decl_block : BEGIN_TYPE type_decl_list END_TYPE
                | BEGIN_TYPE END_TYPE
                | /* empty */
                ;


class_decl_block    : BEGIN_CLASS class_decl_list END_CLASS
                    | BEGIN_CLASS END_CLASS
                    | /* empty */
                    ;

class_decl_list : class_decl_list class_decl 
                | class_decl 
                ;


class_decl  : ID '{' class_field_list class_method_list '}'                 { create_class_table_entry($<s_val>1, $<class_decl_node>3, $<class_decl_node>4, NULL); }
            | ID '{' class_method_list '}'                                  { create_class_table_entry($<s_val>1, NULL, $<class_decl_node>3, NULL); }
            | ID '{' class_field_list '}'                                   { create_class_table_entry($<s_val>1, $<class_decl_node>3, NULL, NULL); }
            | ID EXTENDS ID '{' class_field_list class_method_list '}'      { create_class_table_entry($<s_val>1, $<class_decl_node>5, $<class_decl_node>6, $<s_val>3); }
            | ID EXTENDS ID '{' class_method_list '}'                       { create_class_table_entry($<s_val>1, NULL, $<class_decl_node>5, $<s_val>3); }
            | ID EXTENDS ID '{' class_field_list '}'                        { create_class_table_entry($<s_val>1, $<class_decl_node>5, NULL, $<s_val>3); }
            ;

class_field_list    : BEGIN_DECL class_fields END_DECL  { $<class_decl_node>$ = $<class_decl_node>2; }
                    | BEGIN_DECL END_DECL               { $<class_decl_node>$ = NULL; }
                    ;

class_fields    : class_fields class_field      { $<class_decl_node>$ = join_class_decl_nodes($<class_decl_node>1, $<class_decl_node>2); }
                | class_field                   { $<class_decl_node>$ = $<class_decl_node>1; }
                ;

class_field : class_field_type ID SEMICOLON                             { $<class_decl_node>$ = create_class_decl_field_node($<s_val>2, $<s_val>1); }
            | class_field_type ID '(' func_params_list ')' SEMICOLON    { $<class_decl_node>$ = create_class_decl_method_node($<s_val>2, $<s_val>1, $<decl_node>4, NULL, NULL); }
            | class_field_type ID '(' ')' SEMICOLON                     { $<class_decl_node>$ = create_class_decl_method_node($<s_val>2, $<s_val>1, NULL, NULL, NULL); }
            ;


class_field_type    : INT       { $<s_val>$ = $<s_val>1; }
                    | STR       { $<s_val>$ = $<s_val>1; }
                    | ID        { $<s_val>$ = $<s_val>1; }
                    ;


class_method_list   : class_method_list class_method    { $<class_decl_node>$ = join_class_decl_nodes($<class_decl_node>1, $<class_decl_node>2); }
                    | class_method                      { $<class_decl_node>$ = $<class_decl_node>1; }
                    ;


class_method    : class_field_type ID '(' func_params_list ')' '{' local_decl_block func_body '}'       { $<class_decl_node>$ = create_class_decl_method_node($<s_val>2, $<s_val>1, $<decl_node>4, $<node>8, $<decl_node>7); }
                | class_field_type ID '(' func_params_list ')' '{' func_body '}'                        { $<class_decl_node>$ = create_class_decl_method_node($<s_val>2, $<s_val>1, $<decl_node>4, $<node>7, NULL); }
                | class_field_type ID '(' ')' '{' local_decl_block func_body '}'                        { $<class_decl_node>$ = create_class_decl_method_node($<s_val>2, $<s_val>1, NULL, $<node>7, $<decl_node>6); }
                | class_field_type ID '(' ')' '{' func_body '}'                                         { $<class_decl_node>$ = create_class_decl_method_node($<s_val>2, $<s_val>1, NULL, $<node>6, NULL); }
                ; 


type_decl_list  : type_decl_list type_decl
                | type_decl
                ;

type_decl   : ID '{' type_field_list '}'     { create_user_type_entry($<s_val>1, $<decl_node>3); }
            ;

type_field_list : type_field_list type_field        { $<decl_node>$ = join_decl_nodes($<decl_node>1, $<decl_node>2); }
                | type_field                        { $<decl_node>$ = $<decl_node>1; }
                ; 


type_field  : field_type ID SEMICOLON       { $<decl_node>$ = create_user_type_decl_node($<s_val>2, $<s_val>1); }
            ;



field_type  : INT       { $<s_val>$ = $<s_val>1; }
            | STR       { $<s_val>$ = $<s_val>1; }
            | ID        { $<s_val>$ = $<s_val>1; }
            ;


global_decl_block   : BEGIN_DECL global_decls_list END_DECL
                    | BEGIN_DECL END_DECL
                    | /* empty */
                    ;

global_decls_list   : global_decls_list global_decls
                    | global_decls
                    ;

global_decls    : type gloabl_id_list SEMICOLON
                ;

gloabl_id_list  : gloabl_id_list COMMA global_id 
                | global_id 
                ;

global_id   : ID                                        { create_global_symbol_table_entry($<s_val>1, var_type_entry); }
            | ID '[' NUM ']'                            { create_global_symbol_table_array_entry($<s_val>1, var_type_entry, $<n_val>3, 1); }
            | ID '[' NUM ']' '[' NUM ']'                { create_global_symbol_table_array_entry($<s_val>1, var_type_entry, $<n_val>3, $<n_val>6); }
            | '*' ID                                    { create_global_symbol_table_pointer_entry($<s_val>2, var_type_entry, 1); }
            | ID '(' func_params_list ')'               { create_global_symbol_table_func_entry($<s_val>1, var_type_entry, $<decl_node>3); }
            | ID '(' ')'                                { create_global_symbol_table_func_entry($<s_val>1, var_type_entry, NULL); }
            ;

local_decl_block    : BEGIN_DECL local_decls_list END_DECL      { $<decl_node>$ = $<decl_node>2; }
                    | BEGIN_DECL END_DECL                       { $<decl_node>$ = NULL; }
                    ;

local_decls_list    : local_decls_list local_decls      { $<decl_node>$ = join_decl_nodes($<decl_node>1, $<decl_node>2); }
                    | local_decls                       { $<decl_node>$ = $<decl_node>1; }
                    ;


local_decls     : type local_id_list SEMICOLON          { $<decl_node>$ = $<decl_node>2; }
                ;


local_id_list   : local_id_list COMMA ID    { $<decl_node>$ = join_decl_nodes($<decl_node>1, create_decl_node($<s_val>3, var_type_entry, NULL)); }
                | ID                        { $<decl_node>$ = create_decl_node($<s_val>1, var_type_entry, NULL); }

  
type        : INT           { var_type_entry = default_types->int_type; }
            | STR           { var_type_entry = default_types->str_type; }
            | custom_type   { var_type_entry = $<type_table>1; }
            ;

// custom syntax for tuples
custom_type : TUPLE ID '(' tuple_fields_list ')'                    { $<type_table>$ = create_type_table_entry($<s_val>2, 0, $<decl_node>4, VAR_TYPE_TUPLE); }
            | ID                                                    { $<type_table>$ = get_type_table_entry($<s_val>1); }
            ;


tuple_fields_list   : tuple_fields_list COMMA tuple_field_element   { $<decl_node>$ = join_decl_nodes($<decl_node>1, $<decl_node>3); }
                    | tuple_field_element                           { $<decl_node>$ = $<decl_node>1; }
                    ;

tuple_field_element : tuple_field_type  ID  { $<decl_node>$ = create_decl_node($<s_val>2, tuple_field_type_entry, NULL); num_fields++; }
                    ;

tuple_field_type    : INT   { tuple_field_type_entry = default_types->int_type; }
                    | STR   { tuple_field_type_entry = default_types->str_type; }
                    | ID    { tuple_field_type_entry = get_type_table_entry($<s_val>1); }
                    ;

ret_type    : INT           { return_type_entry = default_types->int_type; }
            | STR           { return_type_entry = default_types->str_type; }
            | custom_type   { return_type_entry = $<type_table>1; }
            ;


//////////////////////////////////////////////////////////////////////////////////////////////////////

func_def_block  : func_def_block func_def       { $<node>$ = create_program_node($<node>1, $<node>2); }
                | func_def                      { $<node>$ = $<node>1; }
                ;


func_def    : ret_type ID '(' func_params_list ')' '{' local_decl_block func_body '}'       { $<node>$ = create_func_body_node($<s_val>2, return_type_entry, $<decl_node>4, $<decl_node>7, $<node>8); }
            | ret_type ID '(' func_params_list ')' '{' func_body '}'                        { $<node>$ = create_func_body_node($<s_val>2, return_type_entry, $<decl_node>4, NULL, $<node>7); }
            | ret_type ID '(' ')' '{' local_decl_block func_body '}'                        { $<node>$ = create_func_body_node($<s_val>2, return_type_entry, NULL, $<decl_node>6, $<node>7); }
            | ret_type ID '(' ')' '{' func_body '}'                                         { $<node>$ = create_func_body_node($<s_val>2, return_type_entry, NULL, NULL, $<node>6); }
            ; 

func_params_list    : func_params_list COMMA func_param     { $<decl_node>$ = join_decl_nodes($<decl_node>1, $<decl_node>3); }
                    | func_param                            { $<decl_node>$ = $<decl_node>1; }
                    ;

func_param      : arg_type ID       { $<decl_node>$ = create_decl_node($<s_val>2, arg_type_entry, NULL); }
                | arg_type '*' ID   { $<decl_node>$ = create_decl_node($<s_val>3, default_types->ptr_type, arg_type_entry); }
                ;

arg_type    : INT           { arg_type_entry = default_types->int_type; }
            | STR           { arg_type_entry = default_types->str_type; }
            | custom_type   { arg_type_entry = $<type_table>1; }
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
            | stmt_init_heap                    { $<node>$ = $<node>1; }
            | stmt_free                         { $<node>$ = $<node>1; }
            | stmt_if                           { $<node>$ = $<node>1; }
            | stmt_while                        { $<node>$ = $<node>1; }
            | stmt_do_while                     { $<node>$ = $<node>1; }
            | stmt_repeat_until                 { $<node>$ = $<node>1; }
            | BREAK                             { $<node>$ = create_break_node(); }
            | CONTINUE                          { $<node>$ = create_continue_node(); }
            | RETURN                            { $<node>$ = create_func_return_node(NULL); }
            | RETURN expr                       { $<node>$ = create_func_return_node($<node>2); }
            | ID '(' ')'                        { $<node>$ = create_func_call_node($<s_val>1, NULL); }
            | ID '(' args_list ')'              { $<node>$ = create_func_call_node($<s_val>1, $<args_node>3); }
            | expr PERIOD func_call             { $<node>$ = create_class_method_node($<node>1, $<node>3); }
            ;

stmt_assign : ID '=' expr                               { $<node>$ = create_assignment_node($<s_val>1, $<node>3); }
            | ID '[' expr ']' '=' expr                  { $<node>$ = create_arr_assignment_node($<s_val>1, $<node>3, $<node>6); }
            | ID '[' expr ']' '[' expr ']' '=' expr     { $<node>$ = create_2d_arr_assignment_node($<s_val>1, $<node>3, $<node>6, $<node>9); }
            | '*' ID '=' expr                           { $<node>$ = create_ptr_assignment_node($<s_val>2, $<node>4); }
            | expr PERIOD ID '=' expr                   { $<node>$ = create_tuple_field_assignment_node($<node>1, $<s_val>3, $<node>5); }
            ;

stmt_write  : WRITE '(' expr ')'                { $<node>$ = create_write_node($<node>3); }
            ;

stmt_read   : READ '(' expr ')'                 { $<node>$ = create_read_node($<node>3); }
            ;

stmt_init_heap : INIT_HEAP '(' ')'             { $<node>$ = create_init_heap_node(); }
                ;

stmt_alloc  : ALLOC '(' ')'                     { $<node>$ = create_alloc_node(); }
            ;

stmt_free   : FREE '(' ID ')'                   { $<node>$ = create_free_node($<s_val>3); }
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
            | '-' expr                          { $<node>$ = create_operator_node($<c_val>1, create_num_node(0), $<node>2); }
            | ID                                { $<node>$ = create_id_node($<s_val>1); }
            | expr GT expr                      { $<node>$ = create_relop_node(">", $<node>1, $<node>3); }
            | expr LT expr                      { $<node>$ = create_relop_node("<", $<node>1, $<node>3); }
            | expr EQUALS expr                  { $<node>$ = create_relop_node("==", $<node>1, $<node>3); }
            | expr NOT_EQUALS expr              { $<node>$ = create_relop_node("!=", $<node>1, $<node>3); }
            | expr GTE expr                     { $<node>$ = create_relop_node(">=", $<node>1, $<node>3); }
            | expr LTE expr                     { $<node>$ = create_relop_node("<=", $<node>1, $<node>3); }
            | expr AND expr                     { $<node>$ = create_relop_node("AND", $<node>1, $<node>3); }
            | expr OR expr                      { $<node>$ = create_relop_node("OR", $<node>1, $<node>3); }
            | STRING_LITERAL                    { $<node>$ = create_string_node($<s_val>1); }
            | '*' expr                          { $<node>$ = create_ptr_ref_node($<node>2); }
            | '&' expr                          { $<node>$ = create_ptr_deref_node($<node>2); }
            | ID '[' expr ']'                   { $<node>$ = create_arr_index_node($<s_val>1, $<node>3); }
            | ID '[' expr ']' '[' expr ']'      { $<node>$ = create_2d_arr_index_node($<s_val>1, $<node>3, $<node>6); }
            | func_call                         { $<node>$ = $<node>1; }
            | expr PERIOD ID                    { $<node>$ = create_tuple_field_node($<node>1, $<s_val>3); }
            | expr PERIOD func_call             { $<node>$ = create_class_method_node($<node>1, $<node>3); }
            | stmt_alloc                        { $<node>$ = $<node>1; }
            | stmt_init_heap                    { $<node>$ = $<node>1; }
            | stmt_free                         { $<node>$ = $<node>1; }    
            | NULLPTR                           { $<node>$ = create_num_node(0); }  
            | NEW '(' ID ')'                    { $<node>$ = create_constructor_node($<s_val>3); }
            ;

func_call   : ID '(' ')'                { $<node>$ = create_func_call_node($<s_val>1, NULL); }
            | ID '(' args_list ')'      { $<node>$ = create_func_call_node($<s_val>1, $<args_node>3); }
            ;

args_list   : args_list COMMA expr              { $<args_node>$ = join_args_nodes($<args_node>1, create_args_node($<node>3)); }
            | expr                              { $<args_node>$ = create_args_node($<node>1); }
            ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "\nError: %s\n", s);
} 

void parser_complete_handler(ast_node_t* node) {
    if (evaluate_mode) {
        // evaluation only implemented till stage 3
    }
    else {
        generate_program(node);
    }
}


int main(int argc, char* argv[]) {
    initialize_type_table();
    create_global_table();
    reset_labels();
    num_fields = 0;
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
                instr_set_fp = fp;
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