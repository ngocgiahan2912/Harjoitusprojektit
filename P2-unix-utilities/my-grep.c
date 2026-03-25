// 27.02.2026
// Han Nguyen
// Project Unix Utilities: my-grep

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Search and print lines containing searchTerm
void searchAndPrint(FILE *file, char *searchTerm) {
    char *aLine = NULL;
    size_t len = 0;

    while (getline(&aLine, &len, file) != -1) {
        if (strstr(aLine, searchTerm) != NULL) { // check if searchTerm is in line
            printf("%s", aLine); // print line
        }
    }
    free(aLine);  // release memory allocated by getline
}

int main(int argc, char *argv[]) {
    if (argc == 1) { // no arguments 
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }

    char *searchTerm = argv[1];

    if (argc == 2) { // only searchterm, no file → read from stdin
        searchAndPrint(stdin, searchTerm);
        return 0;
    }

    for (int i = 2; i < argc; i++) { // loop through each file
        FILE *inputFile;
        inputFile = fopen(argv[i], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "my-grep: cannot open file\n");
            exit(1);
        }
        searchAndPrint(inputFile, searchTerm);
        fclose(inputFile); // always close file after use
    }
    return 0;
}