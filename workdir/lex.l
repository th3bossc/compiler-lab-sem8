%{
#include <stdio.h>
#include <stdlib.h>
#include "./utils/node.h"
#include <stdbool.h>
#include <string.h>
#include "y.tab.h"
%}

%option noyywrap
%%

[0-9]+ {
    node_value_t val;
    val.n_val = atoi(yytext);
    node_t* node = create_node(val, false);
    yylval.node = node;
    
    return NUM;
}

\n {
    return *yytext;
}

[+\-*\/]  {
    node_value_t val;
    val.c_val = *yytext;
    node_t* node = create_node(val, true);
    yylval.node = node;

    return *yytext;
}

[()] {
    return *yytext;
}

%%