%{
    #include <stdio.h>
    #include <stdlib.h>

    void print_operator(char op);

    int pos = 0;
%}

%token DIGIT
%left '+'
%left '*'

%%

start   : expr '\n' { exit(1); }
        ;

expr    : expr '+' expr { print_operator('+'); }
        | expr '*' expr { print_operator('*'); }
        | '(' expr ')'
        | DIGIT { printf("NUM%d ", pos); }
        ;


%% 

void print_operator(char op) {
    switch(op) {
        case '+':
            printf("PLUS ");
            break;
        case '*':
            printf("MUL ");
            break;
    }
}

yyerror(char const* s) {
    printf("yyerror %s\n", s);
}

yylex() {
    char c;
    c = getchar();
    if (isdigit(c)) {
        pos++;
        return DIGIT;
    }  
    else if (c == ' ') {
        yylex();
    }
    else {
        return c;
    }
}

int main() {
    yyparse();
    printf("\n");
    return 0;
}