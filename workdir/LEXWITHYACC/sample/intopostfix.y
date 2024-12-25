%{
    #include <stdio.h>
    #include <stdlib.h>

    void yyerror(const char*);
    int yylex(void);
%}

%token NUM
%left '+' '-'
%left '*' '/'



%%

start   : expr '\n' { printf("\nParse complete\nFinal Result: %d\n", $1); exit(1); }
        ;

expr    : expr '+' expr { printf("+ "); $$ = $1 + $3; }
        | expr '-' expr { printf("- "); $$ = $1 - $3; }
        | expr '*' expr { printf("* "); $$ = $1 * $3; }
        | expr '/' expr { printf("/ "); $$ = $1 / $3; }
        | '(' expr ')'  { $$ = $2; }
        | NUM           { printf("%d ", $1); $$ = $1; }
        ;

%%


void yyerror(const char* s) {
    fprintf(stderr, "%s\n", s);
}



int main() {
    yyparse();
    return 0;
}



