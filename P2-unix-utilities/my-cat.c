// 19.03.2026
// Han Nguyen
// Project Unix Utilities: my-cat

#include <stdio.h>
#include <stdlib.h>

// Read and print content lines
void printFileContent(char fileName[]){
    FILE *InputFile;
    char *aLine = NULL;
    size_t len = 0; 

    //exit if failed
    if ((InputFile = fopen(fileName,"r"))==NULL){
        fprintf(stderr, "my-cat: cannot open file\n");
        exit(1);
    }
    while (getline(&aLine, &len, InputFile) != -1){
        printf("%s",aLine); // loop to print each line of file
    }
    free(aLine);
    fclose(InputFile); 
}

int main(int argc, char *argv[]){
    if (argc == 1){ // exit if no files given
        return 0;
    }
    for (int i = 1; i < argc; i++ ){ //loop throught each file and print and then its content
        printFileContent(argv[i]);
    }
    return 0;
}

