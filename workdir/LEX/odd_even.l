/* Decls */

%{
    #include <stdio.h>
%}


number_sequence [0-9]*


%%

{number_sequence}[24680] {
    printf("Even number [%d]\n", yyleng);
    return atoi(yytext);
}

{number_sequence}[13579] {
    printf("Odd number [%d]\n", yyleng);
    return atoi(yytext);
}

%%


int yywrap() {
    return 1;
}

int main() {
    int num1 = yylex();
    int num2 = yylex();

    int diff = num1-num2;

    if (diff%2)
        printf("You're right");
    else 
        printf("You're wrong");

    printf("\n");
    return 0;
}