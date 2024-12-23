%{
    #include <stdio.h>
%}


condition   A[A-Z][A-Z]

%option noyywrap
%%

{condition} {
    printf("%s\n", yytext);
}

(.)* {
}

%%


int main() {
    FILE* fp = fopen("input_files/words_len_3.txt", "r"); // expected: ABC, AND, APT
    if (fp) {
        yyin = fp;
    }

    yylex();
    return 0;
}