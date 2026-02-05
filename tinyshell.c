#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

int main () {
    char input[MAX_INPUT];
    char* args[MAX_ARGS];
    
    while (1) {
        printf("tsh> ");
        fflush(stdout);

        if(fgets(input, MAX_INPUT, stdin) == NULL) {
            perror("fgets failed");
            continue;
        }


        input[strcspn(input, "\n")] = '\0';

        if(strcmp(input, "exit") == 0) {
            printf("Have a nice day or night!!!");
            break;
        }

        char* token = strtok(input, " ");
        int i = 0;
        
        while (token != NULL && i < MAX_ARGS) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        args[i] = NULL;

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                perror("cd: Missing Arguments");
            } else if(chdir(args[1]) != 0) {
                perror("cd failed");
            }
            continue;
        }

        pid_t pid = fork();

        if(pid == 0) {
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            printf("Exist status: %d\n", status);
        } else {
            perror("fork failed");
        }
    }
    return 0;
}