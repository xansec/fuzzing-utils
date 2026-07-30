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

    while (1) {
        char buf[BUFSIZ];
        ssize_t n = fread(buf, sizeof(char), sizeof(buf), file);
        if (n == 0 && ferror(file)) {
            perror("Unable to read file");
            fclose(file);
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
            perror("Unable to realloc");
            fclose(file);
            exit(1);
        }
        res.contents = tmp;

        memcpy(res.contents + res.len, buf, add);
        res.len = new_len;
    }
    fclose(file);
    return res;
}

#endif // ENTIRE_FILE_UTILS_H
