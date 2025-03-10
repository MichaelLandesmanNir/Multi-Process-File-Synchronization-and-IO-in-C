// Lior Muallem 206438186
// Michael Landesman Nir 314081696

#include "buffered_open.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Buffer size definition
#define BUFFER_SIZE 16384

// Function to wrap the original open function
buffered_file_t *buffered_open(const char *pathname, int flags, ...)
{
    int fd;
    mode_t mode = 0;
    va_list args;

    
    if (flags & O_CREAT)
    {
        // Extract mode from the variable arguments
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }

    // Checking if O_PREAPPEND is specified
    int preappend = 0;
    // Removing the O_PREAPPEND flag from the flags
    if (flags & O_PREAPPEND)
    {
        preappend = 1;
        flags &= ~O_PREAPPEND;
    }

    // Opening the file
    fd = open(pathname, flags, mode);
    if (fd == -1)
    {
        return NULL;
    }

    // Allocating memory for the buffered file structure
    buffered_file_t *bf = (buffered_file_t *)malloc(sizeof(buffered_file_t));
    if (!bf)
    {
        close(fd);
        errno = ENOMEM;
        return NULL;
    }

    // Initializing the buffered file structure
    bf->fd = fd;
    bf->buffer = (char *)malloc(BUFFER_SIZE);
    bf->write_buffer = (char *)malloc(BUFFER_SIZE);
    // Checking if memory allocation failed
    if (!bf->buffer || !bf->write_buffer)
    {
        free(bf->buffer);
        free(bf->write_buffer);
        free(bf);
        close(fd);
        errno = ENOMEM;
        return NULL;
    }
    // Initializing the buffer structure
    bf->buffer_size = BUFFER_SIZE;
    bf->buffer_pos = 0;
    bf->write_buffer_size = 0;
    bf->flags = flags;
    bf->preappend = preappend;

    return bf;
}

// Function to flush the buffer to the file
int buffered_flush(buffered_file_t *bf)
{
    if (bf->buffer_pos == 0)
    {
        return 0;
    }

    // Clear the file contents then write the buffer
    if (lseek(bf->fd, 0, SEEK_SET) == -1)
    {
        return -1;
    }

    ssize_t written = write(bf->fd, bf->buffer, bf->buffer_pos);
    if (written == -1)
    {
        return -1;
    }

    bf->buffer_pos = 0;
    return 0;
}

ssize_t writing(buffered_file_t *bf, const void *buf, size_t count)
{
    // Copy the latest write data to the write_buffer
    size_t write_size = (count < bf->buffer_size) ? count : bf->buffer_size;
    memcpy(bf->write_buffer, buf, write_size);
    bf->write_buffer_size = write_size;

    // Initializing the total written bytes
    size_t total_written = 0;

    // Loop until all bytes are written
    while (count > 0)
    {
        // Calculating the space left in the buffer
        size_t space_left = bf->buffer_size - bf->buffer_pos;
        size_t to_write = (count < space_left) ? count : space_left;

        // Copying the data to the buffer
        memcpy(bf->buffer + bf->buffer_pos, buf, to_write);
        // Updating the buffer position and the buffer pointer
        bf->buffer_pos += to_write;
        // Updating the buffer pointer and the count
        buf = (char *)buf + to_write;
        // Updating the count and the total written bytes
        count -= to_write;
        // Updating the total written bytes
        total_written += to_write;

        // Flush the buffer if it is full
        if (bf->buffer_pos == bf->buffer_size)
        {
            printf("Flushing buffer\n");
            if (buffered_flush(bf) == -1)
            {
                return -1;
            }
        }
    }

    return total_written;
}

// Function to write to the buffered file
ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count)
{
    if (bf->preappend)
    {
        if (handle_preappend(bf, buf, count) == -1)
        {
            return -1;
        }
        // bf->preappend = 0; // Only apply preappend once
        return count;
    }
    else
    {
        return writing(bf, buf, count);
    }
}

// Function to read from the buffered file
ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count)
{
    size_t total_read = 0;

    // Loop until all bytes are read
    while (count > 0)
    {
        // Reading data from the file if the buffer is empty
        if (bf->buffer_pos == 0)
        {
            ssize_t read_size = read(bf->fd, bf->buffer, bf->buffer_size);
            if (read_size == -1)
            {
                return -1;
            }
            if (read_size == 0)
            {
                break; // End of file
            }
            bf->buffer_pos = read_size;
            bf->buffer_pos_read = 0; // Reset the read position
        }

        // Copying data from the buffer to the output buffer
        size_t to_read = (count < (bf->buffer_pos - bf->buffer_pos_read)) ? count : (bf->buffer_pos - bf->buffer_pos_read);
        memcpy(buf, bf->buffer + bf->buffer_pos_read, to_read);

        // Updating positions and counters
        buf = (char *)buf + to_read;
        count -= to_read;
        total_read += to_read;
        bf->buffer_pos_read += to_read;

        // If all data in the buffer has been read, reset buffer_pos
        if (bf->buffer_pos_read == bf->buffer_pos)
        {
            bf->buffer_pos = 0;
            bf->buffer_pos_read = 0;
        }
    }

    return total_read;
}

// Function to close the buffered file
int buffered_close(buffered_file_t *bf)
{
    // Flush the buffer before closing the file
    if (buffered_flush(bf) == -1)
    {
        return -1;
    }

    // Free the buffer and the buffered file structure
    int result = close(bf->fd);
    free(bf->buffer);
    free(bf->write_buffer);
    free(bf);

    return result;
}

// Helper function to handle preappend logic
int handle_preappend(buffered_file_t *bf, const void *buf, size_t count)
{
    // Get the file size
    off_t file_size = lseek(bf->fd, 0, SEEK_END);
    if (file_size == -1)
    {
        return -1;
    }

    // Allocating memory for the temporary buffer
    char *temp_buffer = (char *)malloc(file_size);
    if (!temp_buffer)
    {
        errno = ENOMEM;
        return -1;
    }

    // Reading the file content into the temporary buffer
    lseek(bf->fd, 0, SEEK_SET);
    ssize_t read_size = read(bf->fd, temp_buffer, file_size);
    if (read_size == -1)
    {
        free(temp_buffer);
        return -1;
    }

    // Printing the read data for debugging
    printf("Read Old Data: %.*s\n", (int)read_size, temp_buffer);

    // Truncate the file to clear its contents
    if (ftruncate(bf->fd, 0) == -1)
    {
        free(temp_buffer);
        return -1;
    }

    // Write the new data to the file
    lseek(bf->fd, 0, SEEK_SET);
    ssize_t new_data_written = write(bf->fd, buf, count);
    if (new_data_written == -1)
    {
        free(temp_buffer);
        return -1;
    }

    // Write the old data to the file
    ssize_t old_data_written = write(bf->fd, temp_buffer, read_size);
    if (old_data_written == -1)
    {
        free(temp_buffer);
        return -1;
    }

    free(temp_buffer);
    return new_data_written + old_data_written;
}
