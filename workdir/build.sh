#! /bin/bash

yacc -d parser.y
lex -d lex.l

gcc -g \
    src/code_generation/code_generation.c \
    src/activation_record/activation_record.c \
    src/instr_set/instr_set.c \
    src/label_translation/label_translation.c \
    src/node/class_decl_node.c \
    src/node/args_node.c \
    src/node/ast_node.c \
    src/node/decl_node.c \
    src/symbol_table/global_symbol_table.c \
    src/symbol_table/local_symbol_table.c \
    src/symbol_table/var_details.c \
    src/type_table/type_table.c \
    src/class_table/class_table.c \
    src/class_table/member_field_list.c \
    src/class_table/member_func_list.c \
    lex.yy.c y.tab.c \
    -lfl -o output

rm y.tab.c
rm y.tab.h
rm lex.yy.c

lex label_translator.l

gcc lex.yy.c \
    src/label_translation/label_translation.c \
    -o label_translator

rm lex.yy.c