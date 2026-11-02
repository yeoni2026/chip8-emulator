#pragma once

typedef enum { NATURAL_NUM, PLUS_OPERATOR, MINUS_OPERATOR} Token_type;

typedef struct {
    Token_type token_type;
    char content[24]; 
} Token;

int lexer(char *command, int command_size, Token *token_array);