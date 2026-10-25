#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: %s <file_name>\n", argv[0]);
    }
    char ch8_binary[100];
    for (int i = 0; argv[1][i] != '.'; ++i){
        ch8_binary[i] = argv[1][i];
    }
    strcat(ch8_binary, ".ch8");
    
    FILE *txt_fp = fopen(argv[1], "r");
    FILE *bin_fp = fopen(ch8_binary, "wb");

    while (1){    
        char command[4];
        int idx = 0;
        int size;
        while (idx < 4){
            if (fread(command + idx, sizeof(char), 1, txt_fp) < 1){
                if (feof(txt_fp)){
                    return 0;
                }
                else {
                    fprintf(stderr, "fread Failed.\n");
                    exit(EXIT_FAILURE);
                }
            }
            if (command[idx] == ' ' || command[idx] == '\n' || command[idx] == '\t'){
                if (idx != 0){
                    fprintf(stderr, "syntax Error.\n");
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            ++idx;
        }
        
        int sum = 0;
        for (int i = 3; i >= 0; --i){
            sum *= 16;
            if (command[i] >= '0' && command[i] <= '9'){
                sum += command[i] - '0';
            } else if (command[i] >= 'A' && command[i] <= 'F'){
                sum += command[i] - 'A' + 10;
            } else {
                fprintf(stderr, "syntax Error.\n");
                exit(EXIT_FAILURE);
            }
        }
        fwrite(&sum, sizeof(int), 1, bin_fp);
    }
    fclose(txt_fp);
    fclose(bin_fp);

    return 0;
}