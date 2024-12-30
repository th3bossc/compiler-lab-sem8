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
    yylval.val = atoi(yytext);
    return NUM;
}

[+\-*\/]  {
    yylval.op = *yytext;

    return *yytext;
}

[()=] {
    return *yytext;
}

; {
    return PUNCTUATION;
}

[a-z] {
    yylval.var_name = *yytext;
    return ID;
}

begin { return BEGIN_BLOCK; }
end { return END_BLOCK; }

write { return WRITE; }
read { return READ; }

if { return IF; }
endif { return ENDIF; }
then { return THEN; }
else { return ELSE; }

while { return WHILE; }
endwhile { return ENDWHILE; }

> { return GT; }
>= { return GTE; }
< { return LT; }
<= { return LTE; }
== { return EQUALS; }
!= { return NOT_EQUALS; }


%%