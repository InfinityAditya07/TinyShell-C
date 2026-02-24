#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_HISTORY 100
#define MAX_COMMAND_LENGTH 1024

static char history[MAX_HISTORY][MAX_COMMAND_LENGTH];
static int history_count = 0;
static int current_index = 0;

static struct termios orig_termios;

void disable_raw_mode () {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode () {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void add_to_history(char const *cmd) {
    if (history_count < MAX_HISTORY) {
        strncpy(history[history_count], cmd, MAX_COMMAND_LENGTH - 1);
        history[history_count][MAX_COMMAND_LENGTH - 1] = '\0';
        history_count++;
    }
    current_index = history_count;
}

const char* get_history_up() {
    if (current_index > 0) {
        current_index--;
        return history[current_index];
    }
    return NULL;
}

const char* get_history_down () {
    if (current_index < history_count - 1) {
        current_index++;
        return history[current_index];
    } else {
        current_index = history_count;
        return "";
    }
}

void print_history () {
    for (int i = 0; i < history_count; i++) {
        printf("%d  %s\n", i + 1, history[i]);
    }
}

void read_input(char* buffer) {
    int len = 0;
    buffer[len] = '\0';
    current_index = history_count;

    enable_raw_mode();

    char c;

    while(read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\n') {
            buffer[len] = '\0';
            printf("\n");
            break;
        } else if (c == 127 || c== '\b') {
            if (len > 0) {
                len--;
                buffer[len] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == 27) {
            char seq[2];
            if(read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                if (seq[0] == '[') {
                    if (seq[1] == 'A') {
                        const char*prevCmd = get_history_up();
                        if (prevCmd) {
                            for (int i = 0; i< len; i++) printf("\b \b");
                            len = snprintf(buffer, MAX_INPUT, "%s", prevCmd);
                            printf("%s", buffer);
                            fflush(stdout);
                        }
                    } else if (seq[1] == 'B') {
                        const char* nextCmd = get_history_down();
                        for (int i = 0; i < len; i++) printf("\b \b");
                        if(nextCmd) {
                            len = snprintf(buffer, MAX_INPUT, "%s", nextCmd);
                            printf("%s", buffer);
                            fflush(stdout);
                        } else {
                            len = 0;
                            buffer[0] = '\0';
                            fflush(stdout);
                        }
                    }
                }
            }  
        } else {
            if (len < MAX_INPUT - 1) {
                buffer[len++] = c;
                buffer[len] = '\0';
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }
    disable_raw_mode();
}

int main () {
    char input[MAX_INPUT];
    char* args[MAX_ARGS];
    
    while (1) {
        printf("tsh> ");
        fflush(stdout);

        read_input(input);

        input[strcspn(input, "\n")] = '\0';

        if(strlen(input) == 0) continue;

        add_to_history(input);

        if(strcmp(input, "exit") == 0) {
            printf("Have a nice day or night!!!");
            break;
        }

        char* token = strtok(input, " ");
        int i = 0;
        
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        /* ===================== ADDED SECTION START ===================== */

        /* history command */
        if(strcmp(args[0], "history") == 0) {
            print_history();
            continue;
        }

        /* unset command */
        if(strcmp(args[0], "unset") == 0) {
            if (args[1] == NULL) {
                printf("unset: missing variable name\n");
            } else {
                unsetenv(args[1]);
            }
            continue;
        }

        /* export command */
        if(strcmp(args[0], "export") == 0) {
            if (args[1] == NULL) {
                printf("export: missing argument\n");
            } else {
                char *equal = strchr(args[1], '=');
                if (equal == NULL) {
                    printf("export: invalid format. Use VAR=value\n");
                } else {
                    *equal = '\0';
                    char *name = args[1];
                    char *value = equal + 1;
                    if (setenv(name, value, 1) != 0) {
                        perror("export failed");
                    }
                }
            }
            continue;
        }

        /* ===================== ADDED SECTION END ===================== */

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