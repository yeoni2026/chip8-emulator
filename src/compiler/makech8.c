#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: %s <file_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char ch8_binary[100];
    int k;
    for (k = 0; argv[1][k] != '.'; ++k){
        ch8_binary[k] = argv[1][k];
    }
    ch8_binary[k] = '\0';
    strcat(ch8_binary, ".ch8");
    
    FILE *txt_fp = fopen(argv[1], "r");
    if (txt_fp == NULL){
        fprintf(stderr, "fopen Failed.\n");
        exit(EXIT_FAILURE);
    }
    FILE *bin_fp = fopen(ch8_binary, "wb");
    if (bin_fp == NULL){
        fprintf(stderr, "fopen Failed.\n");
        exit(EXIT_FAILURE);
    }

    while (1){    
        char command[4];
        int idx = 0;
        int size;
        while (idx < 4){
            if ((command[idx] = fgetc(txt_fp)) == EOF){
                fclose(bin_fp);
                
                if (feof(txt_fp)){
                    fclose(txt_fp);
                    return 0;
                }
                else {
                    fclose(txt_fp);
                    fprintf(stderr, "fread Failed.\n");
                    exit(EXIT_FAILURE);
                }
            }
            if (command[idx] == ' ' || command[idx] == '\n' || command[idx] == '\t'){
                if (idx != 0){
                    fclose(txt_fp);
                    fclose(bin_fp);
                    fprintf(stderr, "syntax Error.\n");
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            ++idx;
        }
        
        uint16_t sum = 0;
        int order[4] = {2, 3, 0, 1};
        for (int i = 0; i < 4; ++i){
            sum *= 16;
            if (command[order[i]] >= '0' && command[order[i]] <= '9'){
                sum += command[order[i]] - '0';
            } else if (command[order[i]] >= 'A' && command[order[i]] <= 'F'){
                sum += command[order[i]] - 'A' + 10;
            } else {
                fclose(txt_fp);
                fclose(bin_fp);
                fprintf(stderr, "syntax Error.\n");
                exit(EXIT_FAILURE);
            }
        }
        fwrite(&sum, sizeof(uint16_t), 1, bin_fp);
    }
    fclose(txt_fp);
    fclose(bin_fp);

    return 0;
}