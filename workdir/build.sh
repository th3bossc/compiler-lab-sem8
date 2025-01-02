#! /bin/bash

yacc -d parser.y
lex -d lex.l

gcc -g lex.yy.c y.tab.c \
    core/node.c \
    core/code_generation.c \
    core/label_translation.c \
    core/symbol_table.c \
    core/instr_set.c \
    -lfl -o output

rm y.tab.c
rm y.tab.h
rm lex.yy.c

lex label_translator.l

gcc lex.yy.c \
    core/label_translation.c \
    -o label_translator

rm lex.yy.c