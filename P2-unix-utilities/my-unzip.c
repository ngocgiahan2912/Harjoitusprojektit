// 19.03.2026
// Han Nguyen
// Project Unix Utilities: my-unzip

#include <stdio.h>
#include <stdlib.h>

// unzip RLE file and print
void unzipFile(FILE *inputFile) {
    int count;
    char character;

    // read 5-byte entries 
    while (fread(&count, sizeof(int), 1, inputFile) == 1 &&
           fread(&character, sizeof(char), 1, inputFile) == 1) {
        for (int i = 0; i < count; i++) {
            printf("%c", character); 
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) { 
        fprintf(stderr, "my-unzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) { // loop through each file
        FILE *inputFile = fopen(argv[i], "rb"); //binary mode
        if (inputFile == NULL) {
            fprintf(stderr, "my-unzip: cannot open file\n");
            exit(1);
        }
        unzipFile(inputFile);
        fclose(inputFile); 
    }
    return 0;
}