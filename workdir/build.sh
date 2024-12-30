#! /bin/bash

yacc -d parser.y
lex -d lex.l

gcc lex.yy.c y.tab.c \
    core/node.c \
    core/code_generation.c \
    core/label_translation.c \
    -lfl -o output

rm y.tab.c
rm y.tab.h
rm lex.yy.c

lex label_translator.l

gcc lex.yy.c \
    core/label_translation.c \
    -o label_translator

rm lex.yy.c