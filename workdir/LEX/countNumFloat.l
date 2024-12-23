%{
    #include <stdio.h>
    int count_int;
    int count_float;

    #define INTEGER 1
    #define FLOAT 2
%}

integer -?[0-9]+
decimal -?[0-9]*\.[0-9]+

%option noyywrap
%%

{integer} {
    printf("integer: %d\n", atoi(yytext));
    count_int++;
    // return INTEGER;
}

{decimal} {
    printf("float: %lf\n", atof(yytext));
    count_float++;
    // return FLOAT;
}

%%

int main() {
    count_int = 0;
    count_float = 0;

    FILE* fp = fopen("input_files/num_float.txt", "r");
    if (fp) {
        yyin = fp;
    }

    yylex();

    printf("Num ints: %d\n Num floats: %d\n", count_int, count_float);

    return 0;
}