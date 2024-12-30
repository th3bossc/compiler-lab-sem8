#! /bin/bash

yacc -d parser.y
lex -d lex.l

gcc lex.yy.c y.tab.c core/node.c core/code_generation.c -lfl -o output

rm y.tab.c
rm y.tab.h
rm lex.yy.c
