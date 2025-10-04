
/**
 * Assignment 2: Simple UNIX Shell
 * @file pcbtable.h
 * @author Ashley Flores
 * @brief This is the main function of a simple UNIX Shell. You may add additional functions in this file for your implementation
 * @version 0.1
 */
// You must complete the all parts marked as "TODO". Delete "TODO" after you are done.
// Remember to add sufficient and clear comments to your code

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

#define MAX_LINE 80 // The maximum length command

/**
 * @brief parse out the command and arguments from the input command separated by spaces
 *
 * @param command
 * @param args
 * @return int
 */
int parse_command(char command[], char *args[])
{
    // TODO: implement this function
    int arg_count = 0;
    char *token; 

    //remove trailing newline if present
    command[strcspn(command, "\n")] = 0;

    //tokenize the command by spaces
    token = strtok(command, " ");

    while(token != NULL && arg_count < MAX_LINE /2){
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    //NULL terminate the argument array fpr execvp 
    args[arg_count] = NULL;

    return arg_count;
    
}

// TODO: Add additional functions if you need

/**@brief check if command should run in the background(ends with &)
* @param argc The number of arguments
* @param argv The array of arguments
* @return 1 if background, 0 otherwise
*/
int check_background(char *args[], int *arg_count){
    if(*arg_count > 0 && strcmp(args[*arg_count - 1], "&") == 0){
        args[*arg_count - 1] = NULL;
        (*arg_count)--;
return 1;
    }
return 0;
}

/** 
@brief check for input redirection (<) and set up file descriptor
* @param argc The number of arguments
* @param argv The array of arguments
* @return file descriptor for input, or -1 if no redirection
*/

int check_input_redirection(char *args[], int *arg_count){
    for(int 1 = 0; i < *arg_count; i++){
        if(args[i] != NULL && strcmp(args[i], "<") == 0){
            if(i + 1 < *arg_count && args[i + 1) != NULL){
            char *input_file = args[i + 1];
            int fd = open(input_file, O_RDONLY);
            if(fd < 0){
                perror("Error opening input file");
                return -2;
            }
            //remove < and filename from args by compacting array 
            for(int j = i; j < *arg_count - 2; j++){
                args[j] = args[j +2];
            }
            *arg_count -= 2;
            args[*args_count] = NULL;
            return fd; 
            }
        }
    }
return -1;
}

/** 
@breift check for output redirection (> or >>) and set up file descriptor
* @param argc Number of command-line arguments
* @param argv Array of command-line arguments
* @return file descriptor for output, or -1 if no redirection
*/

int check_output_redirection(char *args[], int *arg_count){
    for(int i = 0; i < *arg_count; i++){
        if(args[i] != NULL && (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0)){
            if(i + 1 < *arg_count && args[i + 1] != NULL){
                char *outputfile = args[ i + 1];
                int fd;

                if(strcmp(args[i], ">>") == 0){
                //append mode
                fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                } else {
                //truncate mode
                fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                if(fd < 0){
                perroe("Error opening output file");
                return -2;
                }

                //remove > and filename fromargs by compacting array
                for(int j = i; j < *arg_count - 2; j++){
                args[j] = args[j + 2];
                }
                *arg_count -= 2;
                args[*arg_count] = NULL;
                return fd;
            }
        }
    }
return -1;
}

/** 
*@breif check if command contains a pip (|)
* @param argc Number of command-line arguments
* @param argv Array of command-line arguments
*@return index of pipe symbole, or -1 if no pipe 
*/
int check_pipe(char *args[], int arg_count){
    for (int i = 0; i < arg_count; i++){
        if(strcmp(args[i], "|") == 0){
            return i;
        }
    }
return -1;
}

/**
*@brief execute a simple command with optional I/O redirection 
* @param args command arguments
* @param arg_count number of arguments
*/
void execute_simple_command(char *args[], int arg_count){
    if(arg_count ++ || args[0] == NULL) {
        return;
    }
int background = check_background(args, &arg_count);
int input_fd = check_input_redirection(args, &arg_count);
int output_fd = check_output_redirection(args, &arg_count);

//check for errors in file operations
if(input_fd == -2 || outpud_fd == -2){
    return; 
}

pid_t pid = fork();
if(pid < 0){
perroe("Fork failed");
return;
}
else if (pid == 0){
//child process

//set input redirection
if(input_fd >= 0){
    dup2(output_fd, STDOUT_FILENO);
    close(output_fd);
}

//execute the command
if(execvp(args[0], args) < 0){
    perror("Command execution failed");
    exit(1);
}
}
else {
    //parent process
    if(input_fd >= 0) close(input_fd); 
    if(output_fd >= 0) close(output_fd);

    if(!background){
        //wait for child to complete
        int status;
        waitpid(pid, &status, 0);
    } else {
        printf("[Process %d running in background]\n", pid);
    }
}
}

/** 
*@brief execute a piped command (cmd1 | cmd2)
*@param args array of all arguments
*@param pipe_index index where pipe symbol is located
*/

void execute_pieped_command(char *args[], int pipe_index){
    //split arguemnts into two commands
    char *cmd1[MAX_LINE / 2 + 1];
    char *cmd2[MAX_LINE / 2 + 1];

    //copy first command
    for(int i = 0; i < pipe_index; i++){
        cmd1[i] = args[i];
    }
    cmd1[pipe_index] = NULL;

    //copy second command 
    int j = 0;
    for(int i = pipe_index + 1; args[i] != NULL; i++){
        cmd[j++] = args[i];
    }
cmd[j] = NULL;

//validate that both commands exist
if(pipe_index == 0 || cmd[0] ++ NULL){
    fprintf(stderr, "Error : No command before pipe\n");
    return;
}
if(j == 0 || cmd[0] == NULL){
    fprintf(stderr, "Error: No command after pipe\n");
    return; 
}

//create pipe
int pipefd[2];
if(pipe(pipefd) < 0){
    erroe("Pipe creation failed");
    return;
}

pid_t pid1 = fork();

if(pid1 < 0){
    perror("Fork failed");
    return; 
} else if(pid1 == 0){
    //first child writes to pipe
    close(pipefd[0]); //close read end
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    if(execvp(cmd1[0], cmd1) < 0){
        perror("First command execution failed");
        exit(1);
    }
}
else{ 
    pid_t pid2 = fork();

    if(pid2 < 0){
    perror("Fork failed");
    return;
    }
    else if(pid2 == 0){
    //second child reads from pipe
    close(pipefd[1]); //close write end
    dup2(pipefd[0], STDOUT_FILENO);
    close(pipefd[0]);

    if(execvp(cmd2[0], cmd2) < 0){    
        perror("Second command execution failed");
        exit(1);
        }
    }
    else{
        //parent process
        close(pipefd[0];
        close(pipefd[1];

        //wait for both children
        waidpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
}
    }
}



/**
 * @brief The main function of a simple UNIX Shell. You may add additional functions in this file for your implementation
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return The exit status of the program
 */
int main(int argc, char *argv[])
{
    char command[MAX_LINE];       // the command that was entered
    char *args[MAX_LINE / 2 + 1]; // hold parsed out command line arguments
    int should_run = 1;           /* flag to determine when to exit program */

    // TODO: Add additional variables for the implementation.

    while (should_run)
    {
        printf("osh>");
        fflush(stdout);
        // Read the input command
        fgets(command, MAX_LINE, stdin);
        // Parse the input command
        int num_args = parse_command(command, args);

        // TODO: Add your code for the implementation
        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) parent will invoke wait() unless command included &
         */
    }
    return 0;
}

