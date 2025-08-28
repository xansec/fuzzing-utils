#ifndef ENTIRE_FILE_UTILS_H
#define ENTIRE_FILE_UTILS_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


// Structure representing the contents of a file in memory
typedef struct EntireFile {
    char* contents;
    size_t len;
} EntireFile;

/*
  Reads an entire file into memory.
  Example usage:

  EntireFile file = read_entire_file_into_memory("some/path/to/file");
  Never fails, exits on error.
*/
inline EntireFile read_entire_file_into_memory(const char* path) {
    EntireFile res = {0};
    if (!path) {
        fprintf(stderr, "No path was given to read\n");
        exit(1);
    }

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Could not open %s for reading\n", path);
        exit(1);
    }

    while (1) {
        char buf[BUFSIZ];
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n == -1) {
            perror("read");
            exit(1);
        }
        if (n == 0) {
            break;
        }
        res.len += n;
        res.contents = (char *)realloc(res.contents, res.len);
        if (res.contents == NULL) {
            perror("malloc");
            exit(1);
        }
        memcpy(res.contents + res.len - n, buf, n);
    }
    close(fd);
    return res;
}

#endif // ENTIRE_FILE_UTILS_H
