#include "parser.h" 
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

int parser(Token *token_array, AST_node *ast_array[]){
    int token_idx = 0;
    int ast_idx = 0;
    int chunk_start = 0;

    while(token_array[token_idx].token_type != TOK_EOF){
        if (token_array[token_idx].token_type == TOK_NEWLINE){
            ast_array[ast_idx] = parser_oper(token_array + chunk_start, token_idx - chunk_start);
            ++ast_idx;
            chunk_start = token_idx;
        }
        ++token_idx;
    }
    
    return ast_idx + 1;
}

static AST_node* parser_oper(Token *token_chunk, int chunk_size){
    int is_in_paren = 0;
    for (int i = chunk_size - 1; i >= 0; --i){
        if (token_chunk[i].token_type == TOK_RPAREN){
            is_in_paren++;
            continue;
        } else if (token_chunk[i].token_type == TOK_LPAREN){
            is_in_paren--;
            continue;
        } 
        if (is_in_paren) continue;
        
        if (token_chunk[i].token_type == TOK_PLUS || token_chunk[i].token_type == TOK_MINUS){
            AST_node* node = malloc(sizeof(AST_node));
            node->left = parser_term(token_chunk, i);
            node->right = parser_term(token_chunk + i + 1, chunk_size - (i + 1));
            node->token = &token_chunk[i];
            return node;
        }
    }
    return parser_term(token_chunk, chunk_size);
}

static AST_node* parser_term(Token *token_chunk, int chunk_size){
    if (chunk_size == 1){    
        AST_node *node = malloc(sizeof(AST_node));
        node->left = NULL;
        node->right = NULL;
        node->token = token_chunk;
        return node;
    } else if (token_chunk[0].token_type == TOK_LPAREN){
        return parser_oper(token_chunk + 1, chunk_size - 2);
    } else {
        //error!
    }
}