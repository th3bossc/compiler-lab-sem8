%{
#include <stdio.h>
#include <stdlib.h>
#include "src/node/ast_node.h"
#include <stdbool.h>
#include <string.h>
#include "y.tab.h"
%}

%option noyywrap
%%

[0-9]+ {
    yylval.n_val = atoi(yytext);
    return NUM;
}

[+\-*\/&%]  {
    yylval.c_val = *yytext;

    return *yytext;
}

[()=\[\]{}] {
    return *yytext;
}

; {
    return SEMICOLON;
}

, {
    return COMMA;
}

\. {
    return PERIOD;
}

int {
    yylval.s_val = strdup(yytext); 
    return INT; 
}
str { 
    yylval.s_val = strdup(yytext);
    return STR; 
}

class { return BEGIN_CLASS; }
endclass { return END_CLASS; }


type { return BEGIN_TYPE; }
endtype { return END_TYPE; }

decl { return BEGIN_DECL; }
enddecl { return END_DECL; }

begin { return BEGIN_BLOCK; }
end { return END_BLOCK; }

write { return WRITE; }
read { return READ; }
alloc { return ALLOC; }
initialize { return INIT_HEAP; }
free { return FREE; }

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
return { return RETURN; }
tuple { return TUPLE; }

AND { return AND; }
OR { return OR; }

null { return NULLPTR; }

self { return SELF; }

">" { return GT; }
">=" { return GTE; }
"<" { return LT; }
"<=" { return LTE; }
"==" { return EQUALS; }
"!=" { return NOT_EQUALS; }

\"([^\"\\]|\\.)*\" {
    yylval.s_val = strdup(yytext);
    return STRING_LITERAL;
}


[a-zA-Z_]([a-zA-Z0-9_]*) {
    yylval.s_val = strdup(yytext);
    return ID;
}

%%