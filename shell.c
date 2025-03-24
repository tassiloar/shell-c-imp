#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 10
#define MAX_PIPES 10
#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
    char buffer[BUFFER_SIZE];

    char *arguments[MAX_PIPES][MAX_ARGS];

    while (1)
    {

        printf("jsh$ ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            break;
        }

        /* check if the input was too long*/
        if (buffer[strlen(buffer) - 1] != '\n')
        {
            fprintf(stderr, "jsh error: Input too long\n");
            continue;
        }

        /* Delete the trailing newline*/
        buffer[strcspn(buffer, "\n")] = '\0';

        char *token;
        char *saveptr;
        int argNum = 0;
        int pipeNum = 0;

        token = strtok_r(buffer, " ", &saveptr);

        while (token != NULL)
        {
            if (strcmp(token, "exit") == 0 && argNum == 0)
            {
                exit(EXIT_SUCCESS);
            }
            if (strcmp(token, "|") == 0)
            {
                arguments[pipeNum][argNum] = NULL;
                pipeNum++;
                argNum = 0;
            }
            else
            {
                if (argNum < MAX_ARGS - 1)
                {
                    arguments[pipeNum][argNum++] = token;
                }
                else
                {
                    fprintf(stderr, "jsh error: Too many arguments \\
                        in a command\n");
                    break;
                }
            }
            token = strtok_r(NULL, " ", &saveptr);
        }
        /* Null  last commands argument list*/
        arguments[pipeNum][argNum] = NULL;

        int total_commands = pipeNum + 1;

        /* File descriptor for the read end of the previous pipe*/
        int prev_fd = -1;
        pid_t pid;

        for (int i = 0; i < total_commands; i++)
        {
            int pipefd[2];

            /* For every command except the last, create a pipe.*/
            if (i < total_commands - 1)
            {
                if (pipe(pipefd) < 0)
                {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            pid = fork();
            if (pid < 0)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {

                /* If this is not the first command, redirect
                 stdin to the previous end.*/
                if (i > 0)
                {
                    if (dup2(prev_fd, STDIN_FILENO) < 0)
                    {
                        perror("dup2: prev_fd -> STDIN");
                        exit(EXIT_FAILURE);
                    }
                }

                /* If this is not the last command, redirect stdout
                to the current pipe's write end.*/

                if (i < total_commands - 1)
                {
                    if (dup2(pipefd[1], STDOUT_FILENO) < 0)
                    {
                        perror("dup2: pipefd[1] -> STDOUT");
                        exit(EXIT_FAILURE);
                    }
                }

                // Close unused file descriptors in the child
                if (prev_fd != -1)
                {
                    close(prev_fd);
                }
                if (i < total_commands - 1)
                {
                    close(pipefd[0]);
                    close(pipefd[1]);
                }

                // Execute the command
                if (arguments[i][0] == NULL)
                {
                    fprintf(stderr, "jsh error: Empty command\n");
                    exit(EXIT_FAILURE);
                }
                execvp(arguments[i][0], arguments[i]);
                perror("execvp");
                exit(EXIT_FAILURE);
            }

            /* In the parent process close the previous
            pipes read end if it exists*/
            if (prev_fd != -1)
            {
                close(prev_fd);
            }
            /* For every command except the last
            set prev_fd to the read end of the current pipe.*/
            if (i < total_commands - 1)
            {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
        }
        // Wait for all child processes to finish.
        for (int i = 0; i < total_commands; i++)
        {
            int status;
            wait(&status);
        }
    }
    return 0;
}
