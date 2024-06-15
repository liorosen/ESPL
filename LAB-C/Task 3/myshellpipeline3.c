#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <linux/limits.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "LineParser.h"
#include <bits/waitflags.h>
#define INPUT_SIZE 2048
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0
#define WCONTINUED 8


typedef struct process{
    cmdLine* cmd;                     /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                       /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	          /* next process in chain */
} process;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
void printProcessList(process** process_list);
void freeProcessList(process* process_list);
void updateProcessStatus(process* process_list, int pid, int status);

// I used the information in here : https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-waitpid-wait-specific-child-process-end
void updateProcessList(process **process_list) {
    process* current = *process_list;
    int status;
    while (current != NULL) {
        pid_t result = waitpid(current->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (result == -1) {
            perror("waitpid");
        } else if (result > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                updateProcessStatus(*process_list, current->pid, TERMINATED);
            } else if (WIFSTOPPED(status)) {
                updateProcessStatus(*process_list, current->pid, SUSPENDED);
            } else if (WIFCONTINUED(status)) {
                updateProcessStatus(*process_list, current->pid, RUNNING);
            }
        }
        current = current->next;
    }
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process* new_process = (process*)malloc(sizeof(process));
    if(!new_process){
        perror("Failed to allocate memory for new process");
        return;
    }
    new_process->cmd = cmd;
    new_process->pid = pid;
    new_process->status = RUNNING;
    if(cmd->blocking){  
        new_process->status = TERMINATED;
    }
    new_process->next = *process_list;  // Setting the head of the list as the next node
    *process_list = new_process;        // The head of the list is the new procees we added
}

/*void printProcessListhelper(process** process_list,process* prev,process* current_link,process* temp,int index){
    if(current_link->status == TERMINATED){
            if(prev == NULL){
                process_list = current_link ->next ;
                temp = current_link;
                current_link = current_link->next;
                freeCmdLines(current_link->cmd);            // Frees the memory allocated for the command line.
                free(current_link);                         // Frees the memory allocated for the process structure.
            }else{
                prev->next = current_link->next;
                temp = current_link;
                current_link =current_link->next;
                freeCmdLines(current_link->cmd);            // Frees the memory allocated for the command line.
                free(current_link);                         // Frees the memory allocated for the process structure.
            }
        }else{
            prev =current_link;
            current_link = current_link->next;
        }
        index++;
}*/

void printProcessListhelper(process** process_list, process* prev, process* current_link) {
    if (current_link->status == TERMINATED) {
        if (prev == NULL) {
            *process_list = current_link->next;
        } else {
            prev->next = current_link->next;
        }
        freeCmdLines(current_link->cmd); // Frees the memory allocated for the command line.
        free(current_link);              // Frees the memory allocated for the process structure.
    }
}

void printProcessList(process** process_list){
    updateProcessList(process_list);

    process* current_link = *process_list;
    process* prev = NULL;
    int index = 0;
    char* status;
    printf("PID\t\t Status\t\t\tCommand\n");

    while (current_link != NULL) {
        if (current_link->status == RUNNING) {
            status = "Running";
        } else if (current_link->status == SUSPENDED) {
            status = "Suspended";
        } else {
            status = "Terminated";
        }

        // Print the process information
        printf(" %d\t\t", current_link->pid);
        printf(" %s\t\t", status);
        printf(" %s\n", current_link->cmd->arguments[0]);

        printProcessListhelper(process_list, prev, current_link);
        prev = current_link;
        current_link = current_link->next;
        index++;
    }
}

void freeProcessList(process* process_list){
    process* current = process_list;
    process* next;
    
    while (current != NULL) {
        next = current->next;
        freeCmdLines(current->cmd);          // Frees the memory allocated for the command line.
        free(current);                       // Frees the memory allocated for the process structure.
        current = next;
    }
}

void updateProcessStatus(process* process_list, int pid, int status) {
    process *currLink = process_list;
    while (currLink != NULL) {
        if (currLink->pid == pid) {
            currLink->status = status;
            return;
        }
        currLink = currLink->next;
    }
}

int openFileWithMode(const char *fileName, const char *mode) {
    int flags;
    if (strcmp(mode, "r") == 0) {
        flags = O_RDONLY;
    } else if (strcmp(mode, "w") == 0) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else {
        fprintf(stderr, "Unsupported file mode: %s\n", mode);
        return -1;
    }
    return open(fileName, flags, 0644);
}

void handleinput(cmdLine *pCmdLine){
    
    // Close the current standard input file descriptor
        if (close(STDIN_FILENO) < 0) {
            perror("close STDIN failed");
            _exit(EXIT_FAILURE);
        }

        // Open the input file
        int inputFile = openFileWithMode(pCmdLine->inputRedirect, "r");
        if (inputFile < 0) {
            perror("open inputRedirect failed");
            _exit(EXIT_FAILURE);
        }

        

        // // Duplicate the input file descriptor to standard input
        // if (openFileWithMode(pCmdLine->inputRedirect, "r") != STDIN_FILENO) {
        //     perror("open inputRedirect to STDIN failed");
        //     _exit(EXIT_FAILURE);
        // }

        // Close the original file descriptor as it's no longer needed
        //close(inputFile);
}

void handleOutput(cmdLine *pCmdLine){
    // Close the current standard output file descriptor
    if (close(STDOUT_FILENO) < 0) {
        perror("close STDOUT failed");
        _exit(EXIT_FAILURE);
    }
  
    // Open the output file
    int outputFile = openFileWithMode(pCmdLine->outputRedirect, "w");
    if (outputFile < 0) {
        perror("open outputRedirect failed");
        _exit(EXIT_FAILURE);
    }

        

        // // Duplicate the output file descriptor to standard output
        // if (openFileWithMode(pCmdLine->outputRedirect, "w") != STDOUT_FILENO) {
        //     perror("open outputRedirect to STDOUT failed");
        //     _exit(EXIT_FAILURE);
        // }

        // Close the original file descriptor as it's no longer needed
        //close(outputFile);
    
}

void handleinputdup(cmdLine *pCmdLine){
    
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

void handleOutputdup(cmdLine *pCmdLine){
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

void sendingMessages(int sig){
     switch (sig) {
        case SIGINT:
            printf("Looper handling SIGINT\n");
            _exit(0);
            break;
        case SIGTSTP:
            printf("Looper handling SIGTSTP\n");
            raise(SIGSTOP);
            break;
        case SIGCONT:
            printf("Looper handling SIGCONT\n");
            break;
    }
} 

void execute(cmdLine *pCmdLine) {
    //I used https://stackoverflow.com/questions/48970420/creating-a-shell-in-c-how-would-i-implement-input-and-output-redirection as reference
    
    signal(SIGINT,  sendingMessages);
    signal(SIGTSTP, sendingMessages);
    signal(SIGCONT, sendingMessages);

    // Redirect input if specified
    if (pCmdLine->inputRedirect) {
        handleinputdup(pCmdLine);
        //handleinput(pCmdLine);
    }
    
    // Redirect output if specified
    if (pCmdLine->outputRedirect) {
       handleOutputdup(pCmdLine);
       //handleOutput(pCmdLine);
    }
      
    // Execute the command
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
        perror("execvp failed");
        _exit(EXIT_FAILURE);
    }
}
 
void alarm_process(cmdLine *pCmdLine,process** process_list) {
    if (pCmdLine->argCount < 2) {
        fprintf(stderr, "alarm: missing process id\n");
        return;
    }

    pid_t pid = atoi(pCmdLine->arguments[1]);
    if (kill(pid, SIGCONT) == -1) {
        perror("alarm failed");
    } else {
        updateProcessStatus(*process_list, pid, RUNNING);
    }
}

void blast_process(cmdLine *pCmdLine,process** process_list) {
    if (pCmdLine->argCount < 2) {
        fprintf(stderr, "blast: missing process id\n");
        return;
    }

    pid_t pid = atoi(pCmdLine->arguments[1]);
    if (kill(pid, SIGINT) == -1) {
        perror("blast failed");
    } else {
        updateProcessStatus(*process_list, pid, TERMINATED);
    }
}

void sleep_process(cmdLine *pCmdLine,process** process_list) {
    if (pCmdLine->argCount < 2) {
        fprintf(stderr, "sleep: missing process id\n");
        return;
    }

    pid_t pid = atoi(pCmdLine->arguments[1]);
    if (kill(pid, SIGTSTP) == -1) {
        perror("sleep failed");
    } else {
        updateProcessStatus(*process_list, pid, SUSPENDED);
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

//Parse a command line input string to execute commands in a shell
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

int handle_alarm_command(cmdLine *parsedLine,process **process_list) {
    if (strcmp(parsedLine->arguments[0], "alarm") == 0) {
        alarm_process(parsedLine, process_list);
        freeCmdLines(parsedLine);
        return 1;
    }
    return 0;
}

int handle_blast_command(cmdLine *parsedLine,process **process_list) {
    if (strcmp(parsedLine->arguments[0], "blast") == 0) {
        blast_process(parsedLine, process_list);
        freeCmdLines(parsedLine);
        return 1;
    }
    return 0;
}

int handle_sleep_command(cmdLine *parsedLine,process **process_list) {
    if (strcmp(parsedLine->arguments[0], "sleep") == 0) {
        sleep_process(parsedLine, process_list);
        freeCmdLines(parsedLine);
        return 1;
    }
    return 0;
}

int handle_procs_command(cmdLine* parsedLine, process** process_list) {
    if (strcmp(parsedLine->arguments[0], "procs") == 0) {
        printProcessList(process_list);
        freeCmdLines(parsedLine);
        return 1;
    }
    return 0;
}

void firstchildCheck(cmdLine *parsedLine, int debug,int pipefd[2],pid_t *child){
    // Fork the first child process
        if ((*child = fork()) == -1) {
            perror("fork failed");
            return;
        }
        if (*child == 0) { // First child process
            close(pipefd[0]); // Close read end of the pipe
            if (dup2(pipefd[1], STDOUT_FILENO) == -1) { // Redirect stdout to pipe
                perror("dup2 failed");
                _exit(EXIT_FAILURE);
            }
            close(pipefd[1]);

            if (debug) {
                fprintf(stderr, "PID: %d\n", getpid());
                fprintf(stderr, "Executing command: %s\n", parsedLine->arguments[0]);
            }

            execute(parsedLine);
            exit(EXIT_SUCCESS); // Ensure the child process exits after executing
        }
}

void secondchildCheck(cmdLine *parsedLine, int debug,int pipefd[2],pid_t *child){
    // Fork the second child process
        if ((*child = fork()) == -1) {
            perror("fork failed");
            return;
        }
        if (*child == 0) { // Second child process
            close(pipefd[1]); // Close write end of the pipe
            if (dup2(pipefd[0], STDIN_FILENO) == -1) { // Redirect stdin to pipe
                perror("dup2 failed");
                _exit(EXIT_FAILURE);
            }
            close(pipefd[0]);

            if (debug) {
                fprintf(stderr, "PID: %d\n", getpid());
                fprintf(stderr, "Executing command: %s\n", parsedLine->arguments[0]);
            }

            execute(parsedLine->next);
            exit(EXIT_SUCCESS); // Ensure the child process exits after executing
        }

}

void parentCheck(cmdLine *parsedLine, int debug,pid_t pid,process** process_list ){
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
        addProcess(process_list, parsedLine, pid);
        if (parsedLine->blocking) {
            waitpid(pid, NULL, 0); // Wait for the child process to complete if it's blocking
        } else {
            printf("Running in background with PID %d\n", pid); // Inform the user that the process is running in the background
        }
    }
       
}

void checkForConflictingRedirections(cmdLine *parsedLine) {
    cmdLine *nextCmd = parsedLine->next;
    // Check for input redirection on the right-hand side of the pipe
    if (nextCmd && nextCmd->inputRedirect) {
        fprintf(stderr, "Error: input redirection on right-hand side of pipe\n");
        exit(EXIT_FAILURE);
    }
    
    // Check for output redirection on the left-hand side of the pipe
    if (parsedLine->outputRedirect && nextCmd) {
        fprintf(stderr, "Error: output redirection on left-hand side of pipe\n");
        exit(EXIT_FAILURE);
    }
}

void fork_and_execute(cmdLine *parsedLine, int debug, process** process_list) {
    int pipefd[2];
    pid_t child1 = -1 ;
    pid_t child2 = -1;

    checkForConflictingRedirections(parsedLine);

    // Handle the "procs" command within the shell
    if (strcmp(parsedLine->arguments[0], "procs") == 0) {
        printProcessList(process_list);
        return;
    }

    // Check if the command contains a pipe
    if (parsedLine->next != NULL) {
        // Create a pipe
        if (pipe(pipefd) == -1) {
            perror("pipe failed");
            return;
        }

        firstchildCheck( parsedLine,  debug, pipefd,&child1);
        secondchildCheck( parsedLine, debug, pipefd,&child2);
    
        // Parent process
        close(pipefd[0]); // Close read end of the pipe
        close(pipefd[1]); // Close write end of the pipe

        // Add both child processes to the process list
        addProcess(process_list, parsedLine, child1);
        addProcess(process_list, parsedLine->next, child2);

        // Wait for both child processes to complete
        waitpid(child1, NULL, 0);
        waitpid(child2, NULL, 0);

    } else {
        // If no pipe, handle the command as usual
        pid_t pid = fork();
        parentCheck(parsedLine,debug,pid,process_list);
    }
}

void process_commands(int debug) {
    char input[INPUT_SIZE];
    cmdLine* parsedLine;
    process* process_list = NULL;


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
            fprintf(stderr, "Failed to parse command line.\n");
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
        if (handle_alarm_command(parsedLine, &process_list)) {
            continue;
        }

        // Check for "sleep" command
        if (handle_sleep_command(parsedLine, &process_list)) {
            break;
        }

        // Check for "blast" command
        if (handle_blast_command(parsedLine, &process_list)) {
            continue;
        }

        // Check for "procs" command
        if (handle_procs_command(parsedLine, &process_list)) {
            continue;
        }

        // Fork a new process to execute the command
        fork_and_execute(parsedLine, debug, &process_list);

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

