#pragma once

typedef enum {
    TOK_NUM,
    TOK_PLUS,
    TOK_MINUS,
    TOK_NEWLINE,
    TOK_EOF
} Token_type;


typedef struct {
    Token_type token_type;
    char *content; 
} Token;

int lexer(char *command, int command_size, Token *token_array);