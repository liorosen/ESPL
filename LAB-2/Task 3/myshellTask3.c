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
        FILE *inputFile = fopen(pCmdLine->inputRedirect, "r");
        if (inputFile == NULL) {
            perror("open inputRedirect failed");
            _exit(EXIT_FAILURE);
        }
        if (dup2(fileno(inputFile), STDIN_FILENO) < 0) {
            perror("dup2 inputRedirect failed");
            _exit(EXIT_FAILURE);
        }
        fclose(inputFile);
    }

    // Redirect output if specified
    if (pCmdLine->outputRedirect) {
        FILE *outputFile = fopen(pCmdLine->outputRedirect, "w");
        if (outputFile == NULL) {
            perror("open outputRedirect failed");
            _exit(EXIT_FAILURE);
        }
        //The output of the command will be written to outputFile
        if (dup2(fileno(outputFile), STDOUT_FILENO) < 0) {
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

void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s> ", cwd);
    } else {
        perror("getcwd() error");
    }
}

int read_input(char *input, size_t size) {
    if (fgets(input, size, stdin) == NULL) {
        perror("fgets() error");
        return -1;
    }
    input[strcspn(input, "\n")] = '\0'; // Remove newline character
    return 0;
}

cmdLine* parse_input(char *input) {
    cmdLine *parsedLine = parseCmdLines(input);
    if (parsedLine == NULL) {
        fprintf(stderr, "parseCmdLines() error: %s\n", strerror(errno));
    }
    return parsedLine;
}

int handle_quit_command(cmdLine *parsedLine) {
    if (strcmp(parsedLine->arguments[0], "quit") == 0) {
        freeCmdLines(parsedLine);
        return 1;
    }
    return 0;
}

int handle_cd_command(cmdLine *parsedLine) {
    if (strcmp(parsedLine->arguments[0], "cd") == 0) {
        change_directory(parsedLine);
        freeCmdLines(parsedLine);
        return 1;
    }
    return 0;
}

int handle_alarm_command(cmdLine *parsedLine) {
    if (strcmp(parsedLine->arguments[0], "alarm") == 0) {
        alarm_process(parsedLine);
        freeCmdLines(parsedLine);
        return 1;
    }
    return 0;
}

int handle_blast_command(cmdLine *parsedLine) {
    if (strcmp(parsedLine->arguments[0], "blast") == 0) {
        blast_process(parsedLine);
        freeCmdLines(parsedLine);
        return 1;
    }
    return 0;
}

void fork_and_execute(cmdLine *parsedLine, int debug) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
    } else if (pid == 0) {
        // Child process
        if (debug) {
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s\n", parsedLine->arguments[0]);
        }
        execute(parsedLine);
        exit(EXIT_SUCCESS); // Ensure the child process exits after executing
    } else {
        // Parent process
        if (parsedLine->blocking) {
            waitpid(pid, NULL, 0); // Wait for the child process to complete if it's blocking
        } else {
            printf("Running in background with PID %d\n", pid); // Inform the user that the process is running in the background
        }
    }
}

void process_commands(int debug) {
    char input[INPUT_SIZE];
    cmdLine *parsedLine;

    while (1) {
        // Display prompt
        display_prompt();

        // Read input
        if (read_input(input, INPUT_SIZE) == -1) {
            continue;
        }

        // Parse input
        parsedLine = parse_input(input);
        if (parsedLine == NULL) {
            continue;
        }

        // Check for "quit" command
        if (handle_quit_command(parsedLine)) {
            break;
        }

        // Check for "cd" command
        if (handle_cd_command(parsedLine)) {
            continue;
        }

        // Check for "alarm" command
        if (handle_alarm_command(parsedLine)) {
            continue;
        }

        // Check for "blast" command
        if (handle_blast_command(parsedLine)) {
            continue;
        }

        // Fork a new process to execute the command
        fork_and_execute(parsedLine, debug);

        // Free the parsed command line
        freeCmdLines(parsedLine);
    }
}

int main(int argc, char *argv[]) {
    int debug = 0;

    // Check for the "-d" flag
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug = 1;
    }

    process_commands(debug);

    return 0;
}

