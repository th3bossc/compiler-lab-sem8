%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "src/label_translation/label_translation.h"

FILE* output_path = NULL;
int instr_count = 0;
int label_count = 0;
bool line_counting_step = true;
%}

label L[0-9]+

%%

[0-9]+\n {
    if (line_counting_step) {
        continue;
    }
    else {
        int num = atoi(yytext);
        fprintf(output_path, "%d\n", num);
    }
}

{label}:\n {
    if (line_counting_step) {
        int label;
        label_count++;
        sscanf(yytext, "L%d:", &label);
        create_label_node(label, instr_count*2 + 2056);
    }
}


(call|CALL)(.*){label}\n  {
    if (line_counting_step) {
        instr_count++;
    }
    else {
        int label;
        sscanf(yytext, "%*s L%d", &label);
        int address = get_label_address(label);
        fprintf(output_path, "CALL %d\n", address);
    }
}

(jmp|JMP)(.*){label}\n {
    if (line_counting_step) {
        instr_count++;
    }
    else {
        int label;
        sscanf(yytext, "%*s L%d", &label);
        int address = get_label_address(label);
        fprintf(output_path, "JMP %d\n", address);
    }
}

(jz|JZ)(.*){label}\n {
    if (line_counting_step) {
        instr_count++;
    }
    else {
        int label;
        int reg_no;
        sscanf(yytext, "%*s R%d, L%d", &reg_no, &label);
        int address = get_label_address(label);
        fprintf(output_path, "JZ R%d, %d\n", reg_no, address);
    }
}

(jnz|JNZ)(.*){label}\n {
    if (line_counting_step) {
        instr_count++;
    }
    else {
        int label;
        int reg_no;
        sscanf(yytext, "%*s R%d, L%d", &reg_no, &label);
        int address = get_label_address(label);
        fprintf(output_path, "JNZ R%d, %d\n", reg_no, address);
    }
}

(.*)\n {
    if (line_counting_step) {
        instr_count++;
    }
    else {
        fprintf(output_path, "%s", yytext);
    }
}



%%

int yywrap() {
    if (line_counting_step) {
        rewind(yyin);
        line_counting_step = false;
    }
    else {
        exit(0);
    }
}


int main(int argc, char* argv[]) {
    label_head_node = NULL;
    label_tail_node = NULL;
    printf("Running label translation....\n");
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0 && (i+1) < argc) {
            FILE* fp = fopen(argv[i+1], "r");
            if (fp) {
                yyin = fp;
            }
        }
        else if (strcmp(argv[i], "--output") == 0 && (i+1) < argc) {
            FILE* fp = fopen(argv[i+1], "w");
            if (fp) {
                output_path = fp;
            }
        }
    }

    yylex();
}