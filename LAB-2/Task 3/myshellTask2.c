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
#include <signal.h>
#define INPUT_SIZE 2048

void execute(cmdLine *pCmdLine) {
    //I used https://stackoverflow.com/questions/48970420/creating-a-shell-in-c-how-would-i-implement-input-and-output-redirection as reference
    // Redirect input if specified
    if (pCmdLine->inputRedirect) {
        int inputFile = fopen(pCmdLine->inputRedirect, "r");
        if (inputFile < 0) {
            perror("open inputRedirect failed");
            _exit(EXIT_FAILURE);
        }
        if (dup2(inputFile, STDIN_FILENO) < 0) {
            perror("dup2 inputRedirect failed");
            _exit(EXIT_FAILURE);
        }
        fclose(inputFile);
    }

    // Redirect output if specified
    if (pCmdLine->outputRedirect) {
        int outputFile = fopen(pCmdLine->outputRedirect, "w");
        if (outputFile < 0) {
            perror("open outputRedirect failed");
            _exit(EXIT_FAILURE);
        }
        if (dup2(outputFile, STDOUT_FILENO) < 0) {
            perror("dup2 outputRedirect failed");
            _exit(EXIT_FAILURE);
        }
        fclose(outputFile);
    }

    // Execute the command
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
        perror("execvp failed");
        _exit(EXIT_FAILURE);
    }
}
 
void alarm_process(cmdLine *pCmdLine) {
    if (pCmdLine->argCount < 2) {
        fprintf(stderr, "alarm: missing process id\n");
        return;
    }

    pid_t pid = atoi(pCmdLine->arguments[1]);
    if (kill(pid, SIGCONT) == -1) {
        perror("alarm failed");
    } else {
        printf("Process %d awakened\n", pid);
    }
}

void blast_process(cmdLine *pCmdLine) {
    if (pCmdLine->argCount < 2) {
        fprintf(stderr, "blast: missing process id\n");
        return;
    }

    pid_t pid = atoi(pCmdLine->arguments[1]);
    if (kill(pid, SIGKILL) == -1) {
        perror("blast failed");
    } else {
        printf("Process %d terminated\n", pid);
    }
}


void change_directory(cmdLine *pCmdLine) {
    if (pCmdLine->argCount < 2) {
        fprintf(stderr, "cd: missing argument\n");
        return;
    }

    if (chdir(pCmdLine->arguments[1]) == -1) {
        perror("cd failed");
        fprintf(stderr, "cd: %s: No such file or directory\n", pCmdLine->arguments[1]);
    } else {
        fprintf(stderr, "Changed directory to: %s\n", pCmdLine->arguments[1]);
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

        // Check for "alarm" command
        if (strcmp(parsedLine->arguments[0], "alarm") == 0) {
            alarm_process(parsedLine);
            freeCmdLines(parsedLine);
            continue;
        }

        // Check for "blast" command
        if (strcmp(parsedLine->arguments[0], "blast") == 0) {
            blast_process(parsedLine);
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
