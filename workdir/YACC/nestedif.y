%{
    #include <stdio.h>
    #include <stdlib.h>
    
    int yylex();
%}

%token IF ELSE LBRACE RBRACE LPARAN RPARAN EXPR

%%

start   : stmts
        ;

stmts   : stmt 
        | stmts stmt 
        ;

stmt    : IF LPARAN EXPR RPARAN LBRACE stmts RBRACE ELSE LBRACE stmts RBRACE
        | IF LPARAN EXPR RPARAN LBRACE stmts RBRACE
        | 
        ;

%% 


yyerror(const char* s) {
    printf("Error: %s\n", s);
}

int main() {
    yyparse();
    return 0;
}