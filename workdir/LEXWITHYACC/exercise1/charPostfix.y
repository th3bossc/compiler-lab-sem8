%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char*);
%}

%token CHAR
%left '+'
%left '*'


%union {
    char c;
}

%%

start   : expr '\n' { printf("\nParse Complete\n"); exit(0); }
        ;

expr    : expr '+' expr { printf("+"); }
        | expr '*' expr { printf("*"); }
        | '(' expr ')'
        | CHAR          { printf("%c", $<c>1); }
        ;
%%

void yyerror(const char* s) {
    fprintf(stderr, "%s\n", s);
}


int main() {
    yyparse();
    return 0;
}