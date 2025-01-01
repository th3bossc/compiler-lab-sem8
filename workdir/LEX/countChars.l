%{
    #include <stdio.h>
    int count_words;
    int count_chars;
    int count_lines;
    int line_with_content;
%}

newline \n
word [^ \n]+

%option noyywrap
%%

{word} {
    count_chars += yyleng;
    count_words++;
    line_with_content = 1;
}

{newline} {
    count_lines++;
    line_with_content = 0;
}

(.) {
    continue;
}

%%

int main() {
    count_words = count_chars = count_lines = 0;
    line_with_content = 0;
    FILE* fp = fopen("input_files/loremipsum.txt", "r");
    if (fp) {
        yyin = fp;
    }

    yylex();
    printf("Num Chars: %d\n", count_chars);
    printf("Num Words: %d\n", count_words);
    printf("Num Lines: %d\n", count_lines+line_with_content);
    return 0;
}