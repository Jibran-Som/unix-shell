# Unix-Style Shell

![C](https://img.shields.io/badge/Language-C-555?style=flat&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Platform-Linux-FCC624?style=flat&logo=linux&logoColor=black)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen)
![License](https://img.shields.io/badge/License-MIT-blue)

---

## Overview

This project implements a functional Unix-style shell in C, providing a command-line interface for executing programs, passing arguments, managing processes, and supporting command pipelines. The shell demonstrates core system programming concepts such as process creation, execution, input parsing, pipelining, and safe memory handling. It is modular, extensible, and designed as a professional portfolio project.

The shell continuously prompts the user for input, parses commands and arguments, executes them in child processes using `fork()` and `execve()`, and handles pipelines between commands. Quoted strings are treated as single arguments, and the shell exits gracefully when the `quit` command is entered.

---

## Features

| Feature | Description |
|---------|-------------|
| Command Execution | Execute single-word commands with the full path (e.g., `/usr/bin/ls`) |
| Command-Line Arguments | Supports multiple arguments including flags and file paths |
| Quoted Strings | Handles spaces in arguments enclosed in quotes as a single argument |
| Pipelining | Supports connecting multiple commands using pipes (`|`) |
| Process Management | Uses `fork()` to create child processes and `wait()` to synchronize |
| Safe Input | Uses `fgets()` to prevent buffer overflow |
| Modular Design | Easily extensible for advanced features like background execution and job control |

---


## Usage

### Building the Shell

Compile the project using:

```bash
make
```

### Running the Shell

Start the shell by running:

```bash
./shell.out
```

## Pipelining

The shell supports a single pipeline connecting two commands for example:

```bash
$ echo narbiJ | rev
$ Jibran
```

## Exiting the Shell

To exit the shell, enter:

```bash
$ quit
```



