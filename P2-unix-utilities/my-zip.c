// 19.03.2026
// Han Nguyen
// Project Unix Utilities: my-zip
// Reference: RLE - https://stackoverflow.com/questions/19748991/compression-program-in-c
// Reference : fwrite- https://man7.org/linux/man-pages/man3/fwrite.3.html

#include <stdio.h>
#include <stdlib.h>

void compressFile(FILE *inputFile) {
    int currChar;
    int nextChar;
    int count;

    currChar = fgetc(inputFile); // read first character
    if (currChar == EOF) return;

    count = 1;

    while ((nextChar = fgetc(inputFile)) != EOF) {
        if (nextChar == currChar) {
            count++; // same character, increment count
        } else {
            // write 4 byte count + 1 character to stdout
            fwrite(&count, sizeof(int), 1, stdout);
            fwrite(&currChar, sizeof(char), 1, stdout);
            currChar = nextChar; // move to next character
            count = 1;
        }
    }
    fwrite(&count, sizeof(int), 1, stdout);
    fwrite(&currChar, sizeof(char), 1, stdout);
}

int main(int argc, char *argv[]) {
    if (argc == 1) { // no files given
        fprintf(stderr, "my-zip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) { // loop through each file
        FILE *inputFile = fopen(argv[i], "rb");
        if (inputFile == NULL) {
            fprintf(stderr, "my-zip: cannot open file\n");
            exit(1);
        }
        compressFile(inputFile);
        fclose(inputFile); 
    }
    return 0;
}