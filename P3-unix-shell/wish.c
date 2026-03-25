// 24.03.2026
// Han Nguyen
// Project 3: Unix Shell (wish)
// Reference: https://en.wikipedia.org/wiki/Unistd.h
// Reference: https://www.man7.org/linux/man-pages/man0/unistd.h.0p.html
// Reference: access() - https://www.geeksforgeeks.org/access-command-in-linux-with-examples/
// Reference: fork() - https://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/create.html
// Reference: dup2() – tiedostokuvaajan kopiointi: https://man7.org/linux/man-pages/man2/dup2.2.html
// Reference: waitpid() – lapsiprosessin päättymisen odottaminen: https://man7.org/linux/man-pages/man2/waitpid.2.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_SIZE 100

// error message as required in the project
char errorMessage[30] = "An error has occurred\n";

// search paths for executables
char *paths[MAX_SIZE];
int pathCount = 0;

// print error to stderr
void printError() {
    write(STDERR_FILENO, errorMessage, strlen(errorMessage));
}

// find full path of a command
char *findPath(char *command) {
    for (int i = 0; i < pathCount; i++) {
        // build full path: path + "/" + cmd
        char *fullPath = malloc(strlen(paths[i]) + strlen(command) + 2);
        if (fullPath == NULL) {
            printError();
            exit(1);
        }
        sprintf(fullPath, "%s/%s", paths[i], command);
        if (access(fullPath, X_OK) == 0) {
            return fullPath; // executable found
        }
        free(fullPath);
    }
    return NULL; // not found
}

// handle exit, when user type "exit"
void handleExit(char **args, int argCount) {
    if (argCount != 1) {
        printError();
        return;
    }
    exit(0);
}

// handle cd
void handleCd(char **args, int argCount) {
    if (argCount != 2) {
        printError();
        return;
    }
    if (chdir(args[1]) != 0) {
        printError();
    }
}

// handle path
// Example:
// user types: path /bin /usr/local/bin
// → paths[0] = "/bin", paths[1] = "/usr/local/bin"
// When executing command "ls", shell first looks for "/bin/ls", 
// if not found, looks for "/usr/local/bin/ls"
void handlePath(char **args, int argCount) {
    // free old paths
    for (int i = 0; i < pathCount; i++) {
        free(paths[i]);
    }
    pathCount = 0;

    // set new paths (safe version)
    for (int i = 1; i < argCount; i++) {
        if (pathCount >= MAX_SIZE) break;

        char *copy = strdup(args[i]);
        if (copy == NULL) {
            printError();
            exit(1);
        }
        paths[pathCount++] = copy;
    }
}

// run a single command with optional redirection
void runCommand(char **args, int argCount, char *outputFile) {
    if (argCount == 0) return;
    
    // check built-in commands
    if (strcmp(args[0], "exit") == 0) {
        handleExit(args, argCount);
        return;
    }
    if (strcmp(args[0], "cd") == 0) {
        handleCd(args, argCount);
        return;
    }
    if (strcmp(args[0], "path") == 0) {
        handlePath(args, argCount);
        return;
    }

    // find executable in path
    char *fullPath = findPath(args[0]);
    if (fullPath == NULL) {
        printError();
        return;
    }

// fork + execv + redirect:
/*
User input: ls -l > output.txt
        |
      Shell
        |
       fork()
       /   \
child process   parent process (shell)
        |
   redirect output -> output.txt
        |
    execv("/bin/ls -l")
        |
  child process ends

Purpose:
- fork() creates a child process, so the main shell can continue receiving commands
- redirect output if the user uses >
- execv() executes the actual command
*/

    // NOTE: fork is handled in executeLine(), so here we only exec

    if (outputFile != NULL) {
        // redirect stdout and stderr to output file
        int outFile = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outFile < 0) {
            printError();
            free(fullPath);
            exit(1);
        }
        dup2(outFile, STDOUT_FILENO);
        dup2(outFile, STDERR_FILENO);
        close(outFile);
    }

    execv(fullPath, args);

    // execv failed
    printError();
    free(fullPath);
    exit(1);
}

// parse and execute a single command (handles redirection)
// Input: "ls -la /tmp > output.txt"
// - Detects '>', splits into command "ls -la /tmp" and file "output.txt"
// - Parses command into args: ["ls", "-la", "/tmp", NULL]
// - Calls runCommand(args, 3, "output.txt")
void parseAndRun(char *cmdStr) {
    char *outputFile = NULL;
    char *args[MAX_SIZE];
    int argCount = 0;

    // check for redirection
    char *redirect = strchr(cmdStr, '>');
    if (redirect != NULL) {
        *redirect = '\0'; // split at '>'
        redirect++;

        // parse output file
        char *token = strtok(redirect, " \t\n");
        if (token == NULL) {
            printError();
            return;
        }
        outputFile = token;

        // check for multiple files after '>'
        if (strtok(NULL, " \t\n") != NULL) {
            printError();
            return;
        }
    }

    // parse command and arguments
    char *token = strtok(cmdStr, " \t\n");
    while (token != NULL && argCount < MAX_SIZE - 1) {
        args[argCount++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[argCount] = NULL; // null-terminate for execv

    runCommand(args, argCount, outputFile);
}

// execute a line (handles parallel commands with '&')
void executeLine(char *line) {
    char *commands[MAX_SIZE];
    int cmdCount = 0;
    pid_t childPids[MAX_SIZE];  
    int childCount = 0;            

    // split by '&' for parallel commands
    char *cmd = strtok(line, "&");
    while (cmd != NULL && cmdCount < MAX_SIZE) {
        commands[cmdCount++] = cmd;
        cmd = strtok(NULL, "&");
    }

    // run all commands in parallel
    for (int i = 0; i < cmdCount; i++) {
        // trim leading whitespace
        char *trimmed = commands[i];
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;

        // built-in commands run directly in parent process (no fork)
        if (strncmp(trimmed, "exit", 4) == 0 ||
            strncmp(trimmed, "cd", 2) == 0 ||
            strncmp(trimmed, "path", 4) == 0) {
            parseAndRun(commands[i]);
            continue;
        }

        // non built-in: fork and execute
        pid_t childPid = fork();    
        if (childPid == 0) {
            // child process - execute the command
            parseAndRun(commands[i]);
            exit(0);
        } else if (childPid > 0) {
            // parent - store child PID
            childPids[childCount++] = childPid;
        } else {
            printError();
        }
    }

    // wait for all child processes
    for (int i = 0; i < childCount; i++) {
        waitpid(childPids[i], NULL, 0);
    }
}

int main(int argc, char *argv[]) {
    // set default path to /bin
    paths[pathCount++] = strdup("/bin");

    FILE *inputFile = stdin; // default: interactive mode

    if (argc == 2) {
        // batch mode
        inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            printError();
            exit(1);
        }
    } else if (argc > 2) {
        // too many arguments
        printError();
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;

    while (1) {
        if (inputFile == stdin) {
            printf("wish> "); // print prompt in interactive mode
            fflush(stdout);
        }

        if (getline(&line, &len, inputFile) == -1) {
            break; // EOF
        }

        executeLine(line);
    }

    free(line);

    // free paths
    for (int i = 0; i < pathCount; i++) {
        free(paths[i]);
    }

    if (inputFile != stdin) fclose(inputFile);

    return 0;
}