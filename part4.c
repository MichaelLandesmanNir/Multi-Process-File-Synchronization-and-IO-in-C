// Lior Muallem 206438186
// Michael Landesman Nir 314081696

#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-l] [-p] <source_directory> <destination_directory>\n", prog_name);
    fprintf(stderr, "  -l: Copy symbolic links as links\n");
    fprintf(stderr, "  -p: Copy file permissions\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int copy_symlinks = 0;
    int copy_permissions = 0;

    // Take command line arguments
    while ((opt = getopt(argc, argv, "lp")) != -1) {
        switch (opt) {
            case 'l':
                copy_symlinks = 1;
                break;
            case 'p':
                copy_permissions = 1;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Extract source and destination directories
    if (optind + 2 != argc) {
        print_usage(argv[0]);
        return 1;
    }

    char *src_dir = argv[optind];
    char *dest_dir = argv[optind + 1];

    // Call your copy_directory function with appropriate arguments
    copy_directory(src_dir, dest_dir, copy_symlinks, copy_permissions);

    return 0;
}