#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


void fristChildPipe(int pipefd[2], pid_t child1){
    // 2. Forking the first child
    child1 = fork();
    if (child1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
 
    // 3. First child process
    else if(child1 == 0){
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");
        close(STDOUT_FILENO);       // 3.1. Close the standard output.
        dup(pipefd[1]);             // 3.2. Duplicate the write-end of the pipe using dup
        close(pipefd[1]);           // 3.3. Close the file descriptor that was duplicated.
        close(pipefd[0]);           // The read end of the pipe- read data from pipe by man

        //3.4. Execute the command "ls -l". 
        char *cmd1[] = {"ls", "-l", NULL};
        fprintf(stderr, "(child1>going to execute cmd: ls -l…)\n");
       if (execvp(cmd1[0], cmd1) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }

    } else {
        fprintf(stderr, "(parent_process>created process with id: %d)\n", child1);
    }

}

void secondChildPipe(int pipefd[2], pid_t child2){
    // 5. Forking the Second child
    child2 = fork();
    if (child2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
 
    // 6. Second child process
    else if(child2 == 0){
        fprintf(stderr, "(child2>redirecting stdout to the write end of the pipe…)\n");
        close(STDIN_FILENO);        // 6.1. Close the standard input.
        dup(pipefd[0]);             // 6.2. Duplicate the read-end of the pipe using dup
        close(pipefd[0]);           // 6.3. Close the file descriptor that was duplicated.
        close(pipefd[0]);           

        // 6.4. Execute the command "tail -n 2". 
        char *cmd2[] = {"tail", "-n" , "2" , NULL};
        fprintf(stderr, "(child2>going to execute cmd: tail  -n …)\n");
        if (execvp(cmd2[0], cmd2) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }

    } else {
        fprintf(stderr, "(parent_process>created process with id: %d)\n", child2);
    }
}

int main() {
    // 1. Creating a pipe
    int pipefd[2];
    pid_t child1 ;
    pid_t child2;

    fprintf(stderr, "(parent_process>forking…)\n");
    // Check for the pipe
    if (pipe(pipefd) == -1) {
        perror("Pipe Failed");
        exit(EXIT_FAILURE);
    }

    fristChildPipe(pipefd,child1);
    
    fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
    close(pipefd[1]);              // 4. Parent closes the write end after forking child1
    
    secondChildPipe(pipefd, child2);

    fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
    close(pipefd[0]);              // 7. Parent closes the write end after forking child2
    
    // 8. wait for the child processes to terminate
    fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    fprintf(stderr, "(parent_process>exiting…)\n");
    return 0;
}