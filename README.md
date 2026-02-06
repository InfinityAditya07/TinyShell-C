# Tiny Shell (tsh) Project

## Overview
This is a simple Linux terminal (shell) implemented in C. It can execute most Linux commands and supports basic shell functionalities like changing directories. The shell uses **fork()**, **execvp()**, and **waitpid()** to manage commands by creating a child process for each command execution.

---

## Features Implemented

1. **Prompt**
   - Displays `tsh>` for user input, similar to a standard terminal.

2. **Read Input**
   - Reads user input using `fgets()` and removes the newline character.

3. **Tokenization**
   - Splits the input into a command and its arguments using `strtok()`.

4. **Built-in Commands**
   - `cd <directory>` → changes the shell's current working directory using `chdir()`.
   - `exit` → exits the shell.

5. **External Commands**
   - Any Linux command that exists as an external program (e.g., `ls`, `pwd`, `cat`) is executed using:
     - `fork()` → creates a child process
     - `execvp()` → replaces child process with the command
     - `waitpid()` → parent waits for child to finish

---

## Commands Supported

### Works Automatically (via fork + execvp)
| Command | Notes |
|---------|-------|
| ls | Lists directory contents |
| pwd | Prints current directory |
| cat <file> | Prints file contents |
| mkdir <dir> | Creates a new directory |
| rm <file> | Deletes a file |
| touch <file> | Creates a new file or updates timestamp |
| echo <text> | Prints text to the screen |
| date | Shows current date and time |
| whoami | Shows current user |
| uname -a | Displays system information |
| cp <src> <dst> | Copies files |
| mv <src> <dst> | Moves or renames files |

### Must Handle Manually (built-ins)
| Command | Reason | Handling |
|---------|--------|---------|
| cd <directory> | Changes shell directory; cannot be run in child process | `chdir(args[1])` |
| exit | Stops the shell; child cannot exit parent | `break` in loop |
| export VAR=value | Changes shell environment; child cannot modify parent | Use `setenv()` (to implement) |
| unset VAR | Removes environment variable | Use `unsetenv()` (to implement) |
| history | Keeps track of past commands | Store in array and display (to implement) |
| alias / unalias | Manage command aliases | Implement manually |
| fg, bg, jobs | Job control managed by shell | Implement manually |

---

## How It Works

1. Print the prompt `tsh>`.
2. Read user input using `fgets()`.
3. Remove the newline character from input.
4. Check for `exit` → break loop if found.
5. Split input into `args[]` array.
6. Check for `cd` → handle manually using `chdir()`.
7. For all other commands:
   - Create a child process using `fork()`.
   - In child → execute command using `execvp()`.
   - In parent → wait for child to finish using `waitpid()`.
8. Loop back to step 1.

---

Future Work

Implement environment variable management (export, unset)
Implement command history
Implement aliasing system
Implement job control (fg, bg, jobs)
Add signal handling (e.g., Ctrl+C, Ctrl+Z)
