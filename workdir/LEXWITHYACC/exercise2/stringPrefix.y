%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char*);
%}

%token STRING

%left '+'
%left '*'

%union {
    char *sVal;
}

%%

start   : expr '\n' { printf("\nParse Complete\n%s\n", $<sVal>1); exit(0); }
        ;

expr    : expr '+' expr { sprintf($<sVal>$, "+ %s %s ", $<sVal>1, $<sVal>3); }
        | expr '*' expr { sprintf($<sVal>$, "* %s %s ", $<sVal>1, $<sVal>3); }
        | '(' expr ')'  { $<sVal>$ = strdup($<sVal>2); }
        | STRING        { $<sVal>$ = strdup($<sVal>1); }
        ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}


int main() {
    yyparse();
    return 0;
}