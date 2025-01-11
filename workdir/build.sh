#! /bin/bash

yacc -d parser.y
lex -d lex.l

gcc -g lex.yy.c y.tab.c \
    src/code_generation/code_generation.c \
    src/instr_set/instr_set.c \
    src/label_translation/label_translation.c \
    src/node/args_node.c \
    src/node/ast_node.c \
    src/node/decl_node.c \
    src/symbol_table/global_symbol_table.c \
    src/symbol_table/local_symbol_table.c \
    src/type_table/type_table.c \
    -lfl -o output

rm y.tab.c
rm y.tab.h
rm lex.yy.c

lex label_translator.l

gcc lex.yy.c \
    src/label_translation/label_translation.c \
    -o label_translator

rm lex.yy.c