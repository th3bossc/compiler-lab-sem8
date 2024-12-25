%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    int yylex(void);
    void yyerror(const char*);
    char buffer[100];
    int pos = 0;
    int newlineSeen = 0;
    int depth = 0;
%}

%token IF THEN ELSE ENDIF BOOLEAN NUM RELOP

%%

start       : stmt  { printf("depth: %d\n", depth); }
            ;


cond        : BOOLEAN
            | NUM RELOP NUM
            ;

stmt        : if_cond else_cond
            | NUM 
            ;

if_cond     : IF '(' cond ')' THEN stmt { depth++; }
            ;

else_cond   : ENDIF
            | ELSE stmt ENDIF
            ;

%% 

int isRelOp(char* str, int length) {
    if (length >= 3)
        return 0;
    if (
        (strcmp(str, ">")   == 0)  ||
        (strcmp(str, "<")   == 0)  ||
        (strcmp(str, "==")  == 0)  ||
        (strcmp(str, "!=")  == 0)  
    )
        return 1;
    else 
        return 0;
}

int yylex() {
    char c;
    c = getchar();
    if (c != '\n' && c != '\t' && c != ' ' && c != '(' && c != ')') {
        buffer[pos++] = c;
        buffer[pos] = '\0';
    }
    if (c == '\n' || c == '(' || c == ')')
        return c;

    if (pos == 0)
        return yylex();

    int token = -1;
    if (strcmp("if", buffer) == 0)
        token = IF;
    else if (strcmp("else", buffer) == 0)
        token = ELSE;
    else if (strcmp("endif", buffer) == 0)
        token = ENDIF;
    else if (strcmp("then", buffer) == 0)
        token = THEN;
    else if ((strcmp("true", buffer) == 0) || strcmp("false", buffer) == 0)
        token = BOOLEAN;
    else if (pos == 1 && isdigit(buffer[0]))
        token = NUM;
    else if (isRelOp(buffer, pos))
        token = RELOP;
    if (token != -1) {
        memset(buffer, '\0', 100*sizeof(char));
        pos = 0;
        return token;
    }
    else {
        return yylex();
    }
}

void yyerror(const char* s) {
    printf("Error: %s\n", s);
}

int main() {
    yyparse();
    return 0;
}