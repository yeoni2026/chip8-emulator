#include <lexer.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    NUM_FLAG, 
    PASS
} Type_flag;

static Token_type get_token_type(char ch){
    switch(ch){
        case '+':
            return TOK_PLUS;
        case '-':
            return TOK_MINUS;
    }
}

int lexer(char *command, int command_size, Token *token_array){
    Type_flag type_flag = PASS;

    int token_index = 0;
    int sum_of_num = 0;
    int line_number = 1;
    for (int i = 0; i < command_size; ++i){
        if (sum_of_num){
            type_flag = NUM_FLAG;
        }

        if (command[i] == '\n' && i != command_size - 1){
            Token token = {TOK_NEWLINE, "new line"};
            token_array[token_index] = token;
            ++token_index;
            ++line_number;
        } else if (isspace(command[i])){
            continue;
        } else if (command[i] >= '0' && command[i] <= '9'){
            sum_of_num += command[i] - '0';
            type_flag = PASS;
        } else if (command[i] == '+' || command[i] == '-'){
            char str[2] = {command[i], '\0'};
            Token token;
            token.token_type = get_token_type(command[i]);
            strcpy(token.content, str);
            token_array[token_index] = token;
            ++token_index;
        } else {
            fprintf(stderr, "Line %d: Unknown symbol: '%c'\n", line_number, command[i]);
            return -1;
        }

        if (type_flag == NUM_FLAG){
            char str[4];
            snprintf(str, sizeof(str), "%d", sum_of_num);
            Token token;
            token.token_type = TOK_NUM;
            strcpy(token.content, str);
            token_array[token_index] = token;
            ++token_index;
        }
    }

    Token token = {TOK_EOF, "EOF"};
    token_array[token_index] = token;
    ++token_index;

    return 0;
}