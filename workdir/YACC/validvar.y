%{
    #include <stdio.h>
    #include <stdlib.h>

    int yylex(void);
    char buffer[100];
    int pos = 0;

%}

%token ID;

%%

start   : ID { printf("id\n"); }
        ;
%%


yylex() {
    char c = getchar();
    if (c == ' ' || c == '\n') {
        if (pos > 0 && !isdigit(buffer[0]))
            return ID;
    }
    else {
        buffer[pos++] = c;
        buffer[pos] = '\0';
        yylex();
    }
}

yyerror(const char* s) {
    fprintf(stderr, "%s\n", s);
}


int main() {
    yyparse();
    return 0;
}