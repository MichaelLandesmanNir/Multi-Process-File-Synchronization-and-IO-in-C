// Lior Muallem 206438186
// Michael Landesman Nir 314081696

#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <dirent.h>
#include <limits.h>

#define MAX_PATH 4096

// Function to copy a file
void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    struct stat stat_buf;
    if (lstat(src, &stat_buf) == -1) {
        perror("lstat failed");
        return;
    }

    // Copy symbolic link as a link
    if (S_ISLNK(stat_buf.st_mode) && copy_symlinks) {
        char link_target[MAX_PATH];
        ssize_t len = readlink(src, link_target, sizeof(link_target) - 1);
        if (len == -1) {
            perror("readlink failed");
            return;
        }
        link_target[len] = '\0';
        if (symlink(link_target, dest) == -1) {
            perror("symlink failed");
            return;
        }
    } else {
        int src_fd = open(src, O_RDONLY);
        if (src_fd == -1) {
            perror("open failed");
            return;
        }

        int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode & 07777);
        if (dest_fd == -1) {
            perror("open failed");
            close(src_fd);
            return;
        }

        char buffer[4096];
        ssize_t bytes;
        while ((bytes = read(src_fd, buffer, sizeof(buffer))) > 0) {
            if (write(dest_fd, buffer, bytes) != bytes) {
                perror("write failed");
                close(src_fd);
                close(dest_fd);
                return;
            }
        }
        if (bytes == -1) {
            perror("read failed");
        }

        close(src_fd);
        close(dest_fd);
    }

    if (copy_permissions && !S_ISLNK(stat_buf.st_mode)) {
        printf("Setting permissions for %s to %o\n", dest, stat_buf.st_mode & 07777);
        if (chmod(dest, stat_buf.st_mode & 07777) == -1) {
            perror("chmod failed");
        }
    }
}

// Helper function to create directories recursively
int create_directories(const char *path) {
    char temp[MAX_PATH];
    snprintf(temp, sizeof(temp), "%s", path);
    char *p = temp;

    while ((p = strchr(p, '/')) != NULL) {
        *p = '\0';
        if (mkdir(temp, 0755) == -1 && errno != EEXIST) {
            perror("mkdir failed");
            return -1;
        }
        *p = '/';
        p++;
    }

    if (mkdir(temp, 0755) == -1 && errno != EEXIST) {
        perror("mkdir failed");
        return -1;
    }
    return 0;
}

// Function to copy a directory
void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    DIR *dir = opendir(src);
    if (!dir) {
        perror("opendir failed");
        return;
    }

    if (create_directories(dest) == -1) {
        closedir(dir);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[MAX_PATH];
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);

        char dest_path[MAX_PATH];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        struct stat stat_buf;
        if (lstat(src_path, &stat_buf) == -1) {
            perror("lstat failed");
            continue;
        }

        if (S_ISDIR(stat_buf.st_mode)) {
            copy_directory(src_path, dest_path, copy_symlinks, copy_permissions);
        } else {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }

        if (copy_permissions && S_ISDIR(stat_buf.st_mode)) {
            printf("Setting permissions for directory %s to %o\n", dest_path, stat_buf.st_mode & 07777);
            if (chmod(dest_path, stat_buf.st_mode & 07777) == -1) {
                perror("chmod failed");
            }
        }
    }

    closedir(dir);
}
