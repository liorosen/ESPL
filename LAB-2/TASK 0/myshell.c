#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "LineParser.h"

#define INPUT_SIZE 2048

void execute(cmdLine *pCmdLine) {
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
        perror("execvp failed");
        _exit(EXIT_FAILURE);
    }
}

int main() {
    char cwd[PATH_MAX];
    char input[INPUT_SIZE];
    cmdLine *parsedLine;

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

        // Fork a new process to execute the command
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            freeCmdLines(parsedLine);
            continue;
        } else if (pid == 0) {
            // Child process
            execute(parsedLine);
        } else {
            // Parent process
            waitpid(pid, NULL, 0);
        }

        // Free the parsed command line
        freeCmdLines(parsedLine);
    }

    return 0;
}
