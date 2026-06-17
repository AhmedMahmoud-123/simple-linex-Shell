# Simple Linux Shell (myShell)

A minimal implementation of a Linux shell that mimics core Linux shell commands and features. This project demonstrates fundamental shell functionality including command execution, I/O redirection, piping, and built-in commands.

## Features

### Core Functionality
- **Command Execution**: Execute external commands in the foreground or background
- **Input/Output Redirection**: Redirect input from files (`<`) and output to files (`>`)
- **Piping**: Connect commands using pipes (`|`) to pass output as input to another command
- **Background Processes**: Run commands in the background using `&`
- **Signal Handling**: Proper handling of Ctrl+C (SIGINT) to terminate foreground processes without killing the shell

### Built-in Commands
- **`cd [directory]`**: Change the current working directory (defaults to home directory)
- **`pwd`**: Print the current working directory
- **`exit [code]`**: Exit the shell with optional exit code
- **`history`**: Display the command history (last 100 commands)

## Project Structure

```
simple-linex-Shell/
├── myShell.c       # Main shell loop and command execution logic
├── builtins.c      # Implementation of built-in commands and history
├── builtins.h      # Header file for built-in commands
├── parser.c        # Command-line argument parsing and redirection handling
├── parser.h        # Header file for parser functions
├── Makefile        # Build configuration
└── README.md       # This file
```

## Building

### Prerequisites
- GCC compiler
- Make
- Standard POSIX development libraries

### Compilation

```bash
make
```

This will compile the shell and produce an executable named `myShell`.

To clean up object files and the executable:

```bash
make clean
```

## Usage

### Running the Shell

```bash
./myShell
```

The shell will display the prompt `myShell> ` and wait for commands.

### Example Commands

```bash
# List files in the current directory
myShell> ls

# Change directory
myShell> cd /home

# Print current working directory
myShell> pwd

# Output redirection - save command output to a file
myShell> ls > output.txt

# Input redirection - read input from a file
myShell> cat < input.txt

# Piping - pass output from one command to another
myShell> ls | grep .c

# Background execution - run a command in the background
myShell> sleep 100 &

# View command history
myShell> history

# Exit the shell
myShell> exit
```

## Implementation Details

### Command Parsing
The parser handles:
- Tokenization of input by whitespace
- Detection of redirection operators (`<`, `>`)
- Identification of piping operators (`|`)
- Background execution indicators (`&`)

### Process Management
- Uses `fork()` to create child processes
- Uses `execvp()` to replace child processes with requested programs
- Implements proper signal handling for Ctrl+C
- Maintains process group separation to prevent signals from affecting the shell

### Built-in Command Handling
Built-in commands are executed directly in the shell process without forking, improving efficiency and allowing state changes (like `cd` which affects the shell's working directory).

### Signal Handling
- SIGINT handler captures Ctrl+C and forwards it only to foreground child processes
- Shell's process group is isolated from child processes to ensure shell survival
- Terminal control is properly managed using `tcsetpgrp()` and `tcgetpgrp()`

## Language Composition

- **C**: 95.9% - Core implementation
- **Makefile**: 4.1% - Build configuration

## Limitations

- Single pipe support (only two commands connected by one pipe)
- No support for multiple input/output redirections in a single command
- No support for logical operators (`&&`, `||`)
- No wildcard expansion or globbing
- No support for quoting or escaping special characters

## Future Enhancements

Potential improvements for a more complete shell implementation:
- Multiple pipe support (chaining more than two commands)
- Advanced redirection (`>>`, `2>`, `2>&1`)
- Logical operators (`&&`, `||`, `;`)
- Command substitution
- Variable expansion
- Wildcard globbing
- Job control (fg, bg, jobs commands)
- Command aliases

## Author

Ahmed Mahmoud

## License

This project is provided as-is for educational purposes.
