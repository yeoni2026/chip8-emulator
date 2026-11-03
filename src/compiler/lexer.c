#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    PUSH_NUM, 
    PUSH_NONE,
    PUSH_OPERATOR,
    PUSH_NEWLINE
} Push_flag;

static Token_type get_token_type(char ch){
    switch(ch){
        case '+':
            return TOK_PLUS;
        case '-':
            return TOK_MINUS;
    }
}

int lexer(char *command, int command_size, Token *token_array){
    Push_flag push_flag = PUSH_NONE;

    int token_index = 0;
    int sum_of_num = 0;
    int line_number = 1;
    for (int i = 0; i < command_size; ++i){
        if (sum_of_num){
            push_flag = PUSH_NUM;
        }

        if (command[i] == '\n' && i != command_size - 1){
            if (push_flag == PUSH_NUM) i -= 1;
            else push_flag = PUSH_NEWLINE;
        } else if (isspace(command[i])){
            if (push_flag != PUSH_NUM) push_flag = PUSH_NONE;
        } else if (command[i] >= '0' && command[i] <= '9'){
            sum_of_num *= 10;
            sum_of_num += command[i] - '0';
            push_flag = PUSH_NONE;
        } else if (command[i] == '+' || command[i] == '-'){
            if (push_flag == PUSH_NUM) i -= 1;
            else push_flag = PUSH_OPERATOR;
        } else {
            fprintf(stderr, "Line %d: Unknown symbol: '%c'\n", line_number, command[i]);
            return -1;
        }

        if (push_flag== PUSH_NUM){
            Token token;
            token.content = malloc(sizeof(char) * 4);
            snprintf(token.content, 4, "%d", sum_of_num);
            token.token_type = TOK_NUM;
            token_array[token_index] = token;
            ++token_index;
            sum_of_num = 0; //초기화
        } else if (push_flag == PUSH_OPERATOR){
            Token token;
            token.content = malloc(sizeof(char) * 2);
            token.content[0] = command[i];
            token.content[1] = '\0';
            token.token_type = get_token_type(command[i]);
            token_array[token_index] = token;
            ++token_index;
        } else if (push_flag == PUSH_NEWLINE){
            Token token = {TOK_NEWLINE, "new line"};
            token_array[token_index] = token; 
            ++token_index;
            ++line_number;
        }
        push_flag = PUSH_NONE;
    }

    if (sum_of_num){
        Token token;
        token.content = malloc(sizeof(char) * 4);
        snprintf(token.content, 4, "%d", sum_of_num);
        token.token_type = TOK_NUM;
        token_array[token_index] = token;
        ++token_index;
    }
    
    Token token = {TOK_EOF, "EOF"};
    token_array[token_index] = token;
    ++token_index;

    return 0;
}