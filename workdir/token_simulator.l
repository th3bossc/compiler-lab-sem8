

%{
    /**
     accepts strings of the form <string><number>
     eg: ab9, a9, asdfdfdf832
    */
    #include <stdio.h>

    #define ID 1
    #define ER 2    
%}


lower_case  [a-z]
upper_case  [A-Z]
number      [0-9]+


%option noyywrap
%%

({lower_case}|{upper_case})+({number}) {
    return ID;
}

(.)* {
    return ER;
}

%%


int main() {
    int token = yylex();
    if (token == ID)    
        printf("Acceptable\n");
    else 
        printf("Not a valid token\n");

    return 0;
}