%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./y.tab.h"
%}

string [a-zA-Z]+

%option noyywrap
%%

{string} {
    strcpy(yylval.sVal, yytext);
    return STRING;    
}

['\n'()*+] {
    return *yytext;
}

[ ] {
    continue;
}

%%