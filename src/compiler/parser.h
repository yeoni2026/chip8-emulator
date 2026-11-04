#pragma once

#include "lexer.h"

typedef struct AST_node {
    Token *token;
    AST_node *left;
    AST_node *right;
} AST_node;

AST_node* parser_oper(AST_node * node);
AST_node* parser_term(AST_node * node);