#! /bin/bash

yacc -d parser.y
lex -d lex.l

gcc lex.yy.c y.tab.c utils/node.c utils/code_generation.c -lfl -o output
