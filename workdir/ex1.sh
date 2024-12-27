#! /bin/bash

yacc parser_ex1.y
lex lex.l
gcc -g lex.yy.c y.tab.c utils/node.c
