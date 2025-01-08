#! /bin/bash

yacc -d parser.y
lex -d lex.l

gcc -g lex.yy.c y.tab.c \
    src/node/node.c \
    src/code_generation/code_generation.c \
    src/label_translation/label_translation.c \
    src/symbol_table/symbol_table.c \
    src/instr_set/instr_set.c \
    -lfl -o output

rm y.tab.c
rm y.tab.h
rm lex.yy.c

lex label_translator.l

gcc lex.yy.c \
    src/label_translation/label_translation.c \
    -o label_translator

rm lex.yy.c