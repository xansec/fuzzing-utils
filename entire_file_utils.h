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
	
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Could not open %s for reading\n", path);
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
            perror("Unable to read fd");
            exit(1);
        }
        if (n == 0) {
            break;
        }
        size_t add = (size_t)n;
        size_t new_len = res.len + add;

        
        char* tmp = (char*)realloc(res.contents, new_len);
        if (tmp == NULL) {
            free(res.contents);
            fprintf(stderr, "realloc failed while reading %s: %s\n", path, strerror(errno));
            close(fd);
            exit(1);
        }
        res.contents = tmp;

        memcpy(res.contents + res.len, buf, add);
        res.len = new_len;
    }
    close(fd);
    return res;
}

#endif // ENTIRE_FILE_UTILS_H
