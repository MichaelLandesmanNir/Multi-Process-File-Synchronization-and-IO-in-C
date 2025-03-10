// Lior Muallem 206438186
// Michael Landesman Nir 314081696

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define FILE_NAME "output.txt"


void write_to_file(const char *message, int count)
{
    // Opening the file for writing in append mode
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Writing the message 'count' times
    for (int i = 0; i < count; i++)
    {
        if (write(fd, message, strlen(message)) == -1)
        {
            perror("Error writing to file");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    // Close the file
    close(fd);
}

int main(int argc, char *argv[])
{
    // Checking that the correct number of arguments is provided
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>\n", argv[0]);
        return 1;
    }

    // Reading command-line arguments
    char *parent_message = argv[1];
    char *child1_message = argv[2];
    char *child2_message = argv[3];
    int count = atoi(argv[4]);

    // Ensuring 'count' is a positive integer
    if (count <= 0)
    {
        fprintf(stderr, "Count must be a positive integer.\n");
        return 1;
    }

    // If file exists, delete it
    if (remove(FILE_NAME) == -1 && errno != ENOENT)
    {
        perror("Error deleting file");
        return 1;
    }

    // Forking two child processes
    for (int i = 0; i < 2; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("Error forking child process");
            return 1;
        }

        // Child process
        if (pid == 0)
        {
            // Child process: delay and then write to file
            sleep(i + 1); // Simulate delay (Naive Syncronization)
            char *message = (i == 0 ? child1_message : child2_message);
            write_to_file(message, count);
            return 0;
        }
    }

    // Parent process waits for both children to finish
    int status;
    for (int i = 0; i < 2; i++)
    {
        if (wait(&status) == -1)
        {
            perror("Error waiting for child process");
            return 1;
        }
    }

    // Parent writes to the file after both children have finished
    write_to_file(parent_message, count);

    return 0;
}
