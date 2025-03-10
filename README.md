# Multi-Process-File-Synchronization-and-I/O-in-C
Explore advanced file synchronization techniques and custom I/O operations in C. The exercises explore various methods of process synchronization, use of file locks, and implementation of a buffered I/O library with a custom pre-append feature.

## Getting Started

To get started with these exercises, clone this repository to your local machine

Prerequisites
Ensure you have the following installed:

GCC (GNU Compiler Collection)
Standard C library
Basic knowledge of Unix/Linux system calls
Compilation
Each part of the exercise can be compiled using the GCC compiler. Example commands for compiling each part are given below:

bash
Copy
gcc -o part1 part1.c
gcc -o part2 part2.c
gcc -o part4 part4.c
gcc -o buffered_open buffered_open.c -lm
gcc -o copytree copytree.c -lm
Exercises Description
Part 1: Naive Synchronization of File Access
File: part1.c

This program demonstrates basic synchronization using sleep calls to prevent interleaving of file writes among parent and child processes.

Usage:

bash
Copy
./part1 "Parent message" "Child1 message" "Child2 message" 10
Part 2: Synchronization Using Lock Files
File: part2.c

Implements file access synchronization among multiple processes using a lock file mechanism.

Usage:

bash
Copy
./part2 "Message 1" "Message 2" "Message 3" 5
Part 4: Directory Copying Utility
File: part4.c

Replicates Python's shutil.copytree functionality in C, capable of copying entire directory structures with support for symbolic links and permissions.

Usage:

bash
Copy
./part4 -l -p <source_directory> <destination_directory>
Buffered I/O with Pre-append Feature
Files: buffered_open.c, buffered_open.h

Provides buffered I/O functionalities with an implementation of a custom O_PREAPPEND flag that allows writing to the beginning of a file.

Example Implementation:

c
Copy
#include "buffered_open.h"
...
buffered_file_t *bf = buffered_open("example.txt", O_WRONLY | O_CREAT | O_PREAPPEND, 0644);
...
buffered_write(bf, "Hello, World!", strlen("Hello, World!"));
...
buffered_close(bf);
Contributors
Lior Muallem
Michael Landesman Nir
