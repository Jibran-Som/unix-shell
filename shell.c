#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "parser.h"

#define BUFLEN 1024
#define MAX_ARGS 64

int main() {
    char buffer[BUFLEN];
    char* parsedinput;
    char* args[MAX_ARGS];
    char fullpath[BUFLEN];

    printf("Welcome to the GroupXX shell! Enter commands, enter 'quit' to exit\n");

    do {
        // Print the terminal prompt and get input
        printf("$ ");
        char *input = fgets(buffer, sizeof(buffer), stdin);
        if (!input) {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }

        // Clean and parse the input string
        parsedinput = (char*) malloc(BUFLEN * sizeof(char));
        if (!parsedinput) {
            fprintf(stderr, "Memory allocation failed\n");
            return -1;
        }

        size_t parselength = trimstring(parsedinput, input, BUFLEN);

        // Exit condition
        if (strcmp(parsedinput, "quit") == 0) {
            printf("Bye!!\n");
            free(parsedinput);
            return 0;
        }

        // Check if input contains a pipe
        char* pipe_pos = strchr(parsedinput, '|');
        
        if (pipe_pos != NULL) {
            // Handle piped commands
            *pipe_pos = '\0'; // Split into two commands at the pipe
            
            char* cmd1 = parsedinput;
            char* cmd2 = pipe_pos + 1;
            
            // Trim whitespace from both commands
            char trimmed_cmd1[BUFLEN];
            char trimmed_cmd2[BUFLEN];
            trimstring(trimmed_cmd1, cmd1, BUFLEN);
            trimstring(trimmed_cmd2, cmd2, BUFLEN);
            
            if (strlen(trimmed_cmd1) == 0 || strlen(trimmed_cmd2) == 0) {
                fprintf(stderr, "Invalid pipe syntax: missing command on one side of pipe\n");
                free(parsedinput);
                continue;
            }
            
            // Tokenize first command
            char* args1[MAX_ARGS];
            char* split1 = strtok(trimmed_cmd1, " ");
            int i1 = 0;
            while (split1 != NULL && i1 < MAX_ARGS - 1) {
                args1[i1++] = split1;
                split1 = strtok(NULL, " ");
            }
            args1[i1] = NULL;
            
            // Tokenize second command
            char* args2[MAX_ARGS];
            char* split2 = strtok(trimmed_cmd2, " ");
            int i2 = 0;
            while (split2 != NULL && i2 < MAX_ARGS - 1) {
                args2[i2++] = split2;
                split2 = strtok(NULL, " ");
            }
            args2[i2] = NULL;
            
            // Create pipe
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("pipe failed");
                free(parsedinput);
                continue;
            }
            
            // Fork first child for command 1
            pid_t pid1 = fork();
            
            if (pid1 < 0) {
                perror("fork failed");
                close(pipefd[0]);
                close(pipefd[1]);
                free(parsedinput);
                continue;
            }
            else if (pid1 == 0) {
                // First child - will execute command 1
                // Redirect stdout to pipe write end
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
                
                // Find and execute command 1
                int path_found1 = 0;
                const char* paths[] = {"/usr/bin/", "/bin/", ""};
                
                for (int j = 0; j < 3; j++) {
                    strcpy(fullpath, paths[j]);
                    strcat(fullpath, args1[0]);
                    
                    if (access(fullpath, X_OK) == 0) {
                        path_found1 = 1;
                        execve(fullpath, args1, NULL);
                        // If execve fails
                        perror("execve failed for first command");
                        exit(EXIT_FAILURE);
                    }
                }
                
                if (!path_found1) {
                    fprintf(stderr, "Command not found: %s\n", args1[0]);
                    exit(EXIT_FAILURE);
                }
            }
            
            // Fork second child for command 2
            pid_t pid2 = fork();
            
            if (pid2 < 0) {
                perror("fork failed");
                close(pipefd[0]);
                close(pipefd[1]);
                free(parsedinput);
                continue;
            }
            else if (pid2 == 0) {
                // Second child - will execute command 2
                // Redirect stdin from pipe read end
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
                
                // Find and execute command 2
                int path_found2 = 0;
                const char* paths[] = {"/usr/bin/", "/bin/", ""};
                
                for (int j = 0; j < 3; j++) {
                    strcpy(fullpath, paths[j]);
                    strcat(fullpath, args2[0]);
                    
                    if (access(fullpath, X_OK) == 0) {
                        path_found2 = 1;
                        execve(fullpath, args2, NULL);
                        // If execve fails
                        perror("execve failed for second command");
                        exit(EXIT_FAILURE);
                    }
                }
                
                if (!path_found2) {
                    fprintf(stderr, "Command not found: %s\n", args2[0]);
                    exit(EXIT_FAILURE);
                }
            }
            
            // Parent process
            close(pipefd[0]);
            close(pipefd[1]);
            
            // Wait for both children to finish
            int status1, status2;
            waitpid(pid1, &status1, 0);
            waitpid(pid2, &status2, 0);
            
        } else {
            // No pipe - handle single command (original code)
            
            // Tokenize the input into args
            char* split = strtok(parsedinput, " ");
            int i = 0;
            while (split != NULL && i < MAX_ARGS - 1) {
                args[i++] = split;
                split = strtok(NULL, " ");
            }
            args[i] = NULL;

            // Try different paths for the command
            const char* paths[] = {
                "/usr/bin/",
                "/bin/",
                ""  // current directory (use with caution)
            };
            
            int path_found = 0;
            for (int j = 0; j < 3; j++) {
                strcpy(fullpath, paths[j]);
                strcat(fullpath, args[0]);
                
                // Check if file exists and is executable
                if (access(fullpath, X_OK) == 0) {
                    path_found = 1;
                    break;
                }
            }

            if (!path_found) {
                fprintf(stderr, "Command not found: %s\n", args[0]);
                free(parsedinput);
                continue;
            }

            // Fork a child process
            pid_t pid = fork();

            if (pid < 0) {
                perror("fork failed");
                free(parsedinput);
                continue;
            }
            else if (pid == 0) {
                // Child process - execute the command with all arguments
                execve(fullpath, args, NULL);
                // If execve fails
                perror("execve failed");
                exit(EXIT_FAILURE);
            }
            else {
                // Parent process waits for child to finish
                int status;
                waitpid(pid, &status, 0);
            }
        }

        // Free the allocated memory
        free(parsedinput);
    } while (1);

    return 0;
}