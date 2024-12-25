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
    yylval.sVal = strdup(yytext);
    return STRING;    
}

['\n'()*+] {
    return *yytext;
}

[ ] {
    continue;
}

%%