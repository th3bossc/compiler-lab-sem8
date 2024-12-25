%{
    #include <stdio.h>
    #include "./y.tab.h"
    #include <string.h>
%}

number [0-9]+
%option noyywrap

%%

{number} {
    yylval = atoi(yytext);
    return NUM;
}

[()] { return *yytext; }

[+*\/\-] { return *yytext; }

\n { return *yytext; }

[ ] { continue; }

%%


