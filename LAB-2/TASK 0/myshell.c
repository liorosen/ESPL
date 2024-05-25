#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "LineParser.h"
#include <sched.h>
#include <linux/limits.h>
#include <sys/wait.h>
#define INPUT_SIZE 2048

void execute(cmdLine *pCmdLine) {
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
        perror("execvp failed");
        _exit(EXIT_FAILURE);
    }
}

void change_directory(cmdLine *pCmdLine) {
    if (pCmdLine->argCount < 2) {
        fprintf(stderr, "cd: missing argument\n");
        return;
    }
    if (chdir(pCmdLine->arguments[1]) == -1) {
        perror("cd failed");
    }
}

int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    char input[INPUT_SIZE];
    cmdLine *parsedLine;
    int debug = 0;

    // Check for the "-d" flag
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug = 1;
    }

    while (1) {
        // Display prompt
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        } else {
            perror("getcwd() error");
            continue;
        }

        // Read input
        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            perror("fgets() error");
            continue;
        }

        // Remove newline character from input
        input[strcspn(input, "\n")] = '\0';

        // Parse input
        parsedLine = parseCmdLines(input);
        if (parsedLine == NULL) {
            perror("parseCmdLines() error");
            continue;
        }

        // Check for "quit" command
        if (strcmp(parsedLine->arguments[0], "quit") == 0) {
            freeCmdLines(parsedLine);
            break;
        }

        // Check for "cd" command
        if (strcmp(parsedLine->arguments[0], "cd") == 0) {
            change_directory(parsedLine);
            freeCmdLines(parsedLine);
            continue;
        }

        // Fork a new process to execute the command
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            freeCmdLines(parsedLine);
            continue;
        } 
        else if (pid == 0) {
            // Child process
            if (debug) {
                fprintf(stderr, "PID: %d\n", getpid());
                fprintf(stderr, "Executing command: %s\n", parsedLine->arguments[0]);
            }
            execute(parsedLine);
        } else {
            // Parent process
            if (parsedLine->blocking) {
                waitpid(pid, NULL, 0); // Wait for the child process to complete if it's blocking
            } else {
                printf("Running in background with PID %d\n", pid); // Inform the user that the process is running in the background
            }
           
           
        }

        // Free the parsed command line
        freeCmdLines(parsedLine);
    }

    return 0;
}
