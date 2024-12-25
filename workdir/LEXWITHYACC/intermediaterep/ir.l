%{
#include <stdio.h>
#include <stdlib.h>
#include "./tree.h"
#include "./y.tab.h"
%}


number  [0-9]+


%option noyywrap


%%

{number} {
    Attribute val;
    val.nVal = atoi(yytext);
    Node* node = initializeNode(val, 1);

    yylval.node = node;
    return NUM;
}

\n {
    return *yytext;
}

[+\-*\/] {
    Attribute val;
    val.cVal = *yytext;
    Node* node = initializeNode(val, 0);

    yylval.node = node;
    return *yytext;
}

[()] {
    return *yytext;
}



[ ] {
    continue;
} 

%%