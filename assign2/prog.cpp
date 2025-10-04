
/**
 * Assignment 2: Simple UNIX Shell
 * @file prog.cpp
 * @author CS433 Student
 * @brief A simple UNIX shell implementation with history, I/O redirection, and pipe support
 * @version 1.0
 */

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

#define MAX_LINE 80
#define HISTORY_SIZE 10

/**
 * @brief Parse the command and arguments from input, handling special operators
 * 
 * @param command The input command string
 * @param args Array to store parsed arguments
 * @return Number of arguments parsed
 */
int parse_command(char command[], char *args[])
{
    int arg_count = 0;
    char *token;
    
    // Remove trailing newline if present
    command[strcspn(command, "\n")] = 0;
    
    // Tokenize the command by spaces
    token = strtok(command, " ");
    
    while (token != NULL && arg_count < MAX_LINE / 2) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    
    // NULL-terminate the argument array for execvp
    args[arg_count] = NULL;
    
    return arg_count;
}

/**
 * @brief Check if command should run in background (ends with &)
 * 
 * @param args Array of command arguments
 * @param arg_count Number of arguments
 * @return 1 if background, 0 otherwise
 */
int check_background(char *args[], int *arg_count)
{
    if (*arg_count > 0 && strcmp(args[*arg_count - 1], "&") == 0) {
        args[*arg_count - 1] = NULL;
        (*arg_count)--;
        return 1;
    }
    return 0;
}

/**
 * @brief Check for input redirection (<) and set up file descriptor
 * 
 * @param args Array of command arguments
 * @param arg_count Pointer to number of arguments
 * @return File descriptor for input, or -1 if no redirection
 */
int check_input_redirection(char *args[], int *arg_count)
{
    for (int i = 0; i < *arg_count; i++) {
        if (args[i] != NULL && strcmp(args[i], "<") == 0) {
            if (i + 1 < *arg_count && args[i + 1] != NULL) {
                char *input_file = args[i + 1];
                int fd = open(input_file, O_RDONLY);
                if (fd < 0) {
                    perror("Error opening input file");
                    return -2;
                }
                // Remove < and filename from args by compacting array
                for (int j = i; j < *arg_count - 2; j++) {
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
 * @brief Check for output redirection (> or >>) and set up file descriptor
 * 
 * @param args Array of command arguments
 * @param arg_count Pointer to number of arguments
 * @return File descriptor for output, or -1 if no redirection
 */
int check_output_redirection(char *args[], int *arg_count)
{
    for (int i = 0; i < *arg_count; i++) {
        if (args[i] != NULL && (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0)) {
            if (i + 1 < *arg_count && args[i + 1] != NULL) {
                char *output_file = args[i + 1];
                int fd;
                
                if (strcmp(args[i], ">>") == 0) {
                    // Append mode
                    fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                } else {
                    // Truncate mode
                    fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                
                if (fd < 0) {
                    perror("Error opening output file");
                    return -2;
                }
                // Remove > and filename from args by compacting array
                for (int j = i; j < *arg_count - 2; j++) {
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
 * @brief Check if command contains a pipe (|)
 * 
 * @param args Array of command arguments
 * @param arg_count Number of arguments
 * @return Index of pipe symbol, or -1 if no pipe
 */
int check_pipe(char *args[], int arg_count)
{
    for (int i = 0; i < arg_count; i++) {
        if (strcmp(args[i], "|") == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Execute a simple command with optional I/O redirection
 * 
 * @param args Command arguments
 * @param arg_count Number of arguments
 */
void execute_simple_command(char *args[], int arg_count)
{
    if (arg_count == 0 || args[0] == NULL) {
        return;
    }
    
    int background = check_background(args, &arg_count);
    int input_fd = check_input_redirection(args, &arg_count);
    int output_fd = check_output_redirection(args, &arg_count);
    
    // Check for errors in file operations
    if (input_fd == -2 || output_fd == -2) {
        return;
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        return;
    }
    else if (pid == 0) {
        // Child process
        
        // Set up input redirection
        if (input_fd >= 0) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        
        // Set up output redirection
        if (output_fd >= 0) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        
        // Execute the command
        if (execvp(args[0], args) < 0) {
            perror("Command execution failed");
            exit(1);
        }
    }
    else {
        // Parent process
        if (input_fd >= 0) close(input_fd);
        if (output_fd >= 0) close(output_fd);
        
        if (!background) {
            // Wait for child to complete
            int status;
            waitpid(pid, &status, 0);
        } else {
            printf("[Process %d running in background]\n", pid);
        }
    }
}

/**
 * @brief Execute a piped command (cmd1 | cmd2)
 * 
 * @param args Array of all arguments
 * @param pipe_index Index where pipe symbol is located
 */
void execute_piped_command(char *args[], int pipe_index)
{
    // Split arguments into two commands
    char *cmd1[MAX_LINE / 2 + 1];
    char *cmd2[MAX_LINE / 2 + 1];
    
    // Copy first command
    for (int i = 0; i < pipe_index; i++) {
        cmd1[i] = args[i];
    }
    cmd1[pipe_index] = NULL;
    
    // Copy second command
    int j = 0;
    for (int i = pipe_index + 1; args[i] != NULL; i++) {
        cmd2[j++] = args[i];
    }
    cmd2[j] = NULL;
    
    // Validate that both commands exist
    if (pipe_index == 0 || cmd1[0] == NULL) {
        fprintf(stderr, "Error: No command before pipe\n");
        return;
    }
    if (j == 0 || cmd2[0] == NULL) {
        fprintf(stderr, "Error: No command after pipe\n");
        return;
    }
    
    // Create pipe
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("Pipe creation failed");
        return;
    }
    
    pid_t pid1 = fork();
    
    if (pid1 < 0) {
        perror("Fork failed");
        return;
    }
    else if (pid1 == 0) {
        // First child: writes to pipe
        close(pipefd[0]); // Close read end
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        if (execvp(cmd1[0], cmd1) < 0) {
            perror("First command execution failed");
            exit(1);
        }
    }
    else {
        pid_t pid2 = fork();
        
        if (pid2 < 0) {
            perror("Fork failed");
            return;
        }
        else if (pid2 == 0) {
            // Second child: reads from pipe
            close(pipefd[1]); // Close write end
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            
            if (execvp(cmd2[0], cmd2) < 0) {
                perror("Second command execution failed");
                exit(1);
            }
        }
        else {
            // Parent process
            close(pipefd[0]);
            close(pipefd[1]);
            
            // Wait for both children
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }
}

/**
 * @brief Main function - implements a simple UNIX shell
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit status
 */
int main(int argc, char *argv[])
{
    char command[MAX_LINE];
    char *args[MAX_LINE / 2 + 1];
    int should_run = 1;
    
    // History feature: store the last command
    char history[MAX_LINE];
    int has_history = 0;
    
    printf("Simple UNIX Shell - CS433 Assignment 2\n");
    printf("Type 'exit' to quit, '!!' to repeat last command\n");
    printf("Supports: I/O redirection (<, >, >>), pipes (|), background (&)\n\n");
    
    while (should_run)
    {
        printf("osh> ");
        fflush(stdout);
        
        // Read the input command
        if (fgets(command, MAX_LINE, stdin) == NULL) {
            break;
        }
        
        // Handle history feature
        if (strcmp(command, "!!\n") == 0) {
            if (!has_history) {
                printf("No commands in history.\n");
                continue;
            }
            // Use the last command
            strcpy(command, history);
            printf("%s", command);
        }
        else {
            // Save current command to history (if not empty)
            if (command[0] != '\n') {
                strcpy(history, command);
                has_history = 1;
            }
        }
        
        // Parse the input command
        int num_args = parse_command(command, args);
        
        // Handle empty command
        if (num_args == 0) {
            continue;
        }
        
        // Check for exit command
        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
            continue;
        }
        
        // Check for pipe
        int pipe_index = check_pipe(args, num_args);
        
        if (pipe_index >= 0) {
            // Execute piped command
            execute_piped_command(args, pipe_index);
        }
        else {
            // Execute simple command (with possible I/O redirection)
            execute_simple_command(args, num_args);
        }
    }
    
    printf("Shell terminated.\n");
    return 0;
}
