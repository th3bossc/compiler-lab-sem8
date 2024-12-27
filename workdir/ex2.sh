yacc -d parser_ex2.y
lex lex.l

gcc y.tab.c lex.yy.c utils/node.c utils/code_generation.c