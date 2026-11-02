#include <lexer.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static const enum Type_flag {NUM_FLAG, PASS};

static Token_type get_token_type(char ch){
    switch(ch){
        case '+':
            return PLUS_OPERATOR;
        case '-':
            return MINUS_OPERATOR;
    }
}

int lexer(char *command, int command_size, Token *token_array){
    enum Type_flag type_flag = PASS;

    int token_index = 0;
    int sum_of_num = 0;
    for (int i = 0; i < command_size; ++i){
        if (sum_of_num){
            type_flag = NUM_FLAG;
        }

        if (command[i] >= '0' && command[i] <= '9'){
            sum_of_num += command[i] - '0';
            type_flag = PASS;
        } else if (command[i] == '+' || command[i] == '-'){ //char형 기호들 모조리
            char str[2] = {command[i], '\0'};
            Token token = {get_token_type(command[i]), str};
            token_array[token_index] = token;
            ++token_index;
        } else {
            fprintf(stderr, "Unknown symbol: '%c'\n", command[i]);
            return -1;
        }

        if (type_flag == NUM_FLAG){
            char str[4];
            snprintf(str, sizeof(str), "%d", sum_of_num);
            Token token = {NATURAL_NUM, str};
            token_array[token_index] = token;
            ++token_index;
        }
    }

    return 0;
}
