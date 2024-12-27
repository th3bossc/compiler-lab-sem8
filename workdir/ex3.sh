#! /bin/bash


yacc -d parser_ex3.y
lex lex.l
gcc y.tab.c lex.yy.c utils/node.c utils/code_generation.c