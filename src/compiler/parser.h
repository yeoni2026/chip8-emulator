#pragma once

#include "lexer.h"

typedef struct AST_node {
    Token *token;
    AST_node *left;
    AST_node *right;
} AST_node;

int parser(Token *token_array, AST_node *ast_array[]); //배열 요소 개수 반환