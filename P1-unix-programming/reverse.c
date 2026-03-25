//26.02.2026
//Han Nguyen
//Project 1: Warmup to C and Unix programming (reverse)

// Reference: Check if two file paths point to the same file using stat()
// https://man7.org/linux/man-pages/man2/stat.2.html
// Reference: Dynamic Memory Allocation in C - realloc()
// https://www.geeksforgeeks.org/c/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void checkSameFile(char *fileA, char *fileB) {
    struct stat stA, stB;
    if (stat(fileA, &stA) == 0 && stat(fileB, &stB) == 0) {
        if (stA.st_ino == stB.st_ino && stA.st_dev == stB.st_dev) {
            fprintf(stderr, "Input and output file must differ\n");
            exit(1);
        }
    }
}

void readLines(FILE *file, char ***lines, int *count) {
    char *aLine = NULL;
    size_t len = 0; //size of aLine

    while (getline(&aLine, &len, file) != -1) {
        aLine[strcspn(aLine, "\n")] = '\0';

        //allocate memory for the curr line
        // + 1 for '\0'
        char *newLine = malloc(strlen(aLine) + 1);
        if (newLine == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        //copy aLine to newLine (to allocated memory) before next iteration overwrite 
        strcpy(newLine, aLine);

        //expand array by 1 pointer slot
        //e.g. [ptr1, ptr2] -> [ptr1, ptr2, ptr3] 
        // each pointer points to one line, count = number of lines (number of pointers)
        char **newArray = realloc(*lines, (*count + 1) * sizeof(char *));
        if (newArray == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        *lines = newArray;
        (*lines)[*count] = newLine;
        (*count)++;
    }
    free(aLine);
}

void printReverse(FILE *out, char **lines, int count) {
    for (int i = count - 1; i >= 0; i--) {
        fprintf(out, "%s\n", lines[i]); // start to print from last line
    }
}

// aLine -> strcpy -> newLine -> assign -> lines[i]
void freeLines(char **lines, int count) {
    for (int i = 0; i < count; i++) {
        free(lines[i]); //free each newLine
    }
    free(lines); // free lines array
}

int main(int argc, char *argv[]) {
    FILE *inputFile = stdin;
    FILE *outputFile = stdout;
    char **lines = NULL;
    int count = 0;

    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if (argc >= 2) {
        inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }
    if (argc == 3) {
        checkSameFile(argv[1], argv[2]);
        outputFile = fopen(argv[2], "w");
        if (outputFile == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            if (inputFile != stdin) fclose(inputFile);
            exit(1);
        }
    }

    readLines(inputFile, &lines, &count);
    printReverse(outputFile, lines, count);
    freeLines(lines, count);

    if (inputFile != stdin) fclose(inputFile);
    if (outputFile != stdout) fclose(outputFile);

    return 0;
}