%{
    #include <stdio.h>
    #include <stdlib.h>

    int isNumber(const char*);

    int depth = 0;
    char buffer[100];
    int pos = 0;
%}


%token IF
%token THEN
%token ELSE
%token RELOP
%token ID 
%token NUM

%%

start   : expr '\n' { printf("\nComplete");exit(1); }
        ;

stmt    : IF '(' expr ')' THEN stmt ELSE stmt   { printf("if else cond\n"); } 
        | IF '(' expr ')' THEN stmt             { printf("if cond\n"); }
        | id
        ;

expr    : true
        | false
        | cond '==' cond 
        ;

cond    : ID
        | NUM
        ;

%% 


int isNumber(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}


yylex() {
    char c;
    c = getchar();
    buffer[pos++] = c;
    buffer[pos] = '\0';
    if (strcmp("if", buffer)) {
        pos = 0;
        return IF;
    }
    else if (strcmp("then", buffer)) {
        pos = 0;
        return THEN;
    }
    else if (strcmp("else", buffer))  {
        pos = 0;
        return ELSE;
    }
    else {
        yylex();
    }
}

yyerror(const char* s) {
    printf("Error: %s\n", s);
}

int main() {
    yyparse();
    return 0;
}