Tiny Shell (tsh) Project

Overview

Tiny Shell (tsh) is a custom Linux shell implemented in C.
It mimics basic terminal behavior and supports both built-in and external commands.

The shell uses the following system calls and libraries:

fork() → Create child processes

execvp() → Execute external commands

waitpid() → Wait for child process completion

termios → Enable raw mode for advanced input handling

This project demonstrates core Operating System concepts such as process creation, environment variable management, and terminal control.

Features Implemented
1. Custom Prompt

Displays the following prompt before every command:

tsh>

2. Raw Mode Input (Arrow Key Support)

Input is handled using raw terminal mode (termios) instead of fgets().

This enables:

Character-by-character input

Backspace support

Up/Down arrow key navigation through command history

Raw mode disables:

Canonical mode (ICANON)

Echo (ECHO)

3. Command History

Stores up to 100 previous commands

Navigate history using:

Up Arrow (↑) → Previous command

Down Arrow (↓) → Next command

history command prints all previously executed commands

4. Built-in Commands

These commands are handled directly inside the shell (without fork()):

| Command            | Description                 | Implementation     |
| ------------------ | --------------------------- | ------------------ |
| `cd <dir>`         | Change directory            | `chdir()`          |
| `exit`             | Exit the shell              | `break` from loop  |
| `history`          | Print command history       | Stored in 2D array |
| `export VAR=value` | Set environment variable    | `setenv()`         |
| `unset VAR`        | Remove environment variable | `unsetenv()`       |

5. Environment Variable Management
Export
export NAME=value

Splits input at =

Calls setenv(name, value, 1)

Unset
unset NAME

Calls unsetenv(name)

6. External Command Execution

All non built-in commands are executed using:

fork();
execvp();
waitpid();

Example supported commands:

ls
pwd
cat file
mkdir dir
rm file
touch file
echo text
date
whoami
uname -a
cp src dst
mv src dst

If execution fails:

perror() displays the error

Child process exits with EXIT_FAILURE

Program Flow

Print prompt tsh>

Enable raw mode

Read input character-by-character

Support:

Backspace

Arrow key history navigation

Disable raw mode

Add command to history

Tokenize input using strtok()

Check for built-in commands

If not built-in:

fork()

execvp()

waitpid()

Print exit status

Repeat loop

Data Structures Used
History Storage
history[MAX_HISTORY][MAX_COMMAND_LENGTH];

Maximum 100 commands

Each command up to 1024 characters

Argument Storage
char* args[MAX_ARGS];

Maximum 64 arguments per command

Compilation
gcc tinyshell.c -o tsh
Run
./tsh
Exit

To exit the shell:

exit

The shell terminates gracefully.

Technical Concepts Demonstrated

Process creation (fork)

Program execution (execvp)

Process synchronization (waitpid)

Environment variable management (setenv, unsetenv)

Terminal raw mode (termios)

Manual command parsing

History management using arrays

Basic shell architecture