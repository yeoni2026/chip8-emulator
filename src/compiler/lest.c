#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"


int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL){
        fprintf(stderr, "Cannot open the file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = malloc(sizeof(char) * size);
    fread(buffer, 1, size, fp);
    fclose(fp);

    Token *token_array = malloc(sizeof(Token) * 1024);
    int res = lexer(buffer, size, token_array);
    if (res == -1) exit(EXIT_FAILURE);
    
    free(buffer);
    free(token_array);
    
    return 0;
}