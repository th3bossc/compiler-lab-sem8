%{
#include <stdio.h>
#include <stdlib.h>
#include "core/node.h"
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

[+\-*\/&%]  {
    yylval.op = *yytext;

    return *yytext;
}

[()=\[\]] {
    return *yytext;
}

; {
    return SEMICOLON;
}

, {
    return COMMA;
}


decl { return BEGIN_DECL; }
enddecl { return END_DECL; }

int { return INT; }
str { return STR; }

begin { return BEGIN_BLOCK; }
end { return END_BLOCK; }

write { return WRITE; }
read { return READ; }

if { return IF; }
endif { return ENDIF; }
then { return THEN; }
else { return ELSE; }

while { return WHILE; }
do { return DO; }
endwhile { return ENDWHILE; }

repeat { return REPEAT; }
until { return UNTIL; }
endrepeat { return ENDREPEAT; }

break { return BREAK; }
continue { return CONTINUE; }

">" { return GT; }
">=" { return GTE; }
"<" { return LT; }
"<=" { return LTE; }
"==" { return EQUALS; }
"!=" { return NOT_EQUALS; }

\"([^\"\\]|\\.)*\" {
    yylval.var_name = strdup(yytext);
    return STRING_LITERAL;
}


[a-zA-Z_]([a-zA-Z0-9_]*) {
    yylval.var_name = strdup(yytext);
    return ID;
}

%%