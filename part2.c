// Lior Muallem 206438186
// Michael Landesman Nir 314081696

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define LOCKFILE "lockfile.lock"

// Function to write a message to stdout 'count' times
void write_message(const char *message, int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", message);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

// Function to acquire the lock by creating a lock file
void acquire_lock()
{
    while (open(LOCKFILE, O_CREAT | O_EXCL, 0644) == -1)
    {
        if (errno != EEXIST)
        {
            perror("Error creating lock file");
            exit(EXIT_FAILURE);
        }
        usleep(1000); // Wait for 1 millisecond before retrying
    }
}

// Function to release the lock by deleting the lock file
void release_lock()
{
    if (remove(LOCKFILE) == -1)
    {
        perror("Error removing lock file");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    // Require at least 3 arguments: 2 messages and 1 count
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <count>\n", argv[0]);
        return 1;
    }

    // Extract the count (last argument)
    int count = atoi(argv[argc - 1]);
    if (count <= 0)
    {
        fprintf(stderr, "Count must be a positive integer.\n");
        return 1;
    }

    // If lock file exists, delete it
    if (remove(LOCKFILE) == -1 && errno != ENOENT)
    {
        perror("Error deleting lock file");
        return 1;
    }

    int num_children = argc - 2; // The number of messages (First one is the program name, last one is the count)

    pid_t pids[num_children];

    // Forking child processes
    for (int i = 0; i < num_children; i++)
    {
        pids[i] = fork();

        if (pids[i] == -1)
        {
            perror("Error forking child");
            return 1;
        }

        if (pids[i] == 0)
        {
            // Child process: Acquire lock, write message, release lock
            acquire_lock();
            write_message(argv[i + 1], count);
            release_lock();
            return 0; // Child process exits
        }
    }

    // Parent process waits for all children to finish
    for (int i = 0; i < num_children; i++)
    {
        int status;
        if (waitpid(pids[i], &status, 0) == -1)
        {
            perror("Error waiting for child");
            return 1;
        }
    }

    return 0;
}
