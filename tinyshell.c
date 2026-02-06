#include <stdio.h>             //This is used for input output funtion like printf(), fgets(), perror()
#include <stdlib.h>            //This is used for exit(), EXIT_FAILURE
#include <string.h>           //This is used for string handling like strcmp(), strtok(), strcspn()
#include <sys/types.h>        //Defines data types like pid_t
#include <unistd.h>           // for shell programs gives fork(), execvp(), chdir()
#define MAX_INPUT 1024       //Maximum characters user can type in one command
#define MAX_ARGS 64         //Maximum number of arguments a command can have

int main () {
    char input[MAX_INPUT];   //Stores what user types
    char* args[MAX_ARGS];    //It stores command + arguments as an array of strings
    
    while (1) {             //Infinite Loop
        printf("tsh> ");    // Prints prompt like a real terminal
        fflush(stdout);     //ensures it prints immediately

        if(fgets(input, MAX_INPUT, stdin) == NULL) {     //Reads user input from keyboard
            perror("fgets failed");
            continue;
        }


        input[strcspn(input, "\n")] = '\0';     //Remove newline from input

        if(strcmp(input, "exit") == 0) {
            printf("Have a nice day or night!!!");      //If user types exit → loop stops → program ends
            break;
        }

        char* token = strtok(input, " ");     //splits the input by space and just run for one time and give the first token to while loop.
        int i = 0;
        
        while (token != NULL && i < MAX_ARGS) {      //This will take make the token like args[0] = "ls" and so on.
            args[i++] = token;
            token = strtok(NULL, " ");                     
        }

        args[i] = NULL;     //execvp() requires last argument to be NULL

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                perror("cd: Missing Arguments");     //we have do this thing sepratly because fork will not work in this child process will never excuite this thing.
            } else if(chdir(args[1]) != 0) {
                perror("cd failed");
            }
            continue;    //continue will move the cursior to the top skipped the code below it because we don't have to fork.
        }

        pid_t pid = fork();

        if(pid == 0) {
            execvp(args[0], args);    //In this code we are making the parent child process for the command exectution.
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

//Cannot run (built-ins)	They must change the shell itself, but fork + execvp runs in a child which cannot affect the shell
// Can run (external)	They are real programs that run separately; fork + execvp runs them fine without needing to touch the shell

// These are the command we can run:-
// pwd	
// cat	
// echo	
// mkdir	
// rm	
// touch	
// date	
// whoami	
// uname	
// cp	
// mv	

// These are the command we cannot run:-
// cd /path
// exit
// export VAR=value
// unset VAR
// history
// alias name=cmd
// unalias name
// fg, bg, jobs