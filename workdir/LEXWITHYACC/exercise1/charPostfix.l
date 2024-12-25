%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./y.tab.h"
%}


char    [a-zA-Z]
op      [+*] 

%option noyywrap
%%

{char} {
    yylval.c = *yytext;
    return CHAR;
}

{op} {
    return *yytext;
}

[ ] {
    continue;
}

[()] {
    return *yytext;
}

\n {
    return *yytext;
}

%%