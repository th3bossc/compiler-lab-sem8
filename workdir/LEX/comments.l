%{
    #include <stdio.h>
%}

single_line_comment \/\/.*\n
multi_line_comment \/\*([^\*]|(\*[^\/]))*\*\/

%option noyywrap
%%

{single_line_comment} {
    printf("single line: %s\n", yytext);
}

{multi_line_comment} {
    printf("multi line: %s\n", yytext);
}

(.)* {
    printf("N / A: %s\n", yytext);
}

%%

int main() {
    FILE* fp = fopen("input_files/comments.txt", "r");
    if (fp) {
        yyin = fp;
    }

    yylex();
    return 0;
}