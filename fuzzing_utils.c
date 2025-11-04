#include "fuzzing_utils.h"
#include "simple_fdp.h"
#include "entire_file_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

/* Provide global provider pointer */
simple_fdp* global_provider = NULL;

/* Simple dynamic array for registered tests */
static FuzzTestFunc* tests = NULL;
static size_t tests_count = 0;
static size_t tests_capacity = 0;

void RegisterFuzzTest(FuzzTestFunc func) {
    if (!func) return;
    if (tests_count == tests_capacity) {
        size_t new_cap = tests_capacity ? tests_capacity * 2 : 8;
        FuzzTestFunc* new_arr = (FuzzTestFunc*)realloc(tests, new_cap * sizeof(FuzzTestFunc));
        if (!new_arr) {
            fprintf(stderr, "RegisterFuzzTest: out of memory\n");
            return;
        }
        tests = new_arr;
        tests_capacity = new_cap;
    }
    tests[tests_count++] = func;
}

static int is_dir(const char* p) {
    struct stat st;
    return (stat(p, &st) == 0) && S_ISDIR(st.st_mode);
}
static int is_regfile(const char* p) {
    struct stat st;
    return (stat(p, &st) == 0) && S_ISREG(st.st_mode);
}

void RunFuzzTests(const char* file_path) {
    if (!file_path) {
        fprintf(stderr, "RunFuzzTests: file_path is NULL\n");
        return;
    }
    if (!is_dir(file_path) && !is_regfile(file_path)) {
        fprintf(stderr, "Error: %s does not exist or is not a file/dir\n", file_path);
        return;
    }

    /* gather files */
    char** files = NULL;
    size_t files_cnt = 0, files_cap = 0;

    if (is_dir(file_path)) {
        DIR* d = opendir(file_path);
        if (!d) {
            fprintf(stderr, "opendir(%s) failed: %s\n", file_path, strerror(errno));
            return;
        }
        struct dirent* e;
        while ((e = readdir(d)) != NULL) {
            if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
            /* build path */
            size_t need = strlen(file_path) + 1 + strlen(e->d_name) + 1;
            char* full = (char*)malloc(need);
            if (!full) continue;
            strcpy(full, file_path);
            if (file_path[strlen(file_path)-1] != '/') strcat(full, "/");
            strcat(full, e->d_name);
            if (is_regfile(full)) {
                if (files_cnt == files_cap) {
                    size_t ncap = files_cap ? files_cap*2 : 8;
                    char** na = (char**)realloc(files, ncap * sizeof(char*));
                    if (!na) { free(full); break; }
                    files = na; files_cap = ncap;
                }
                files[files_cnt++] = full;
            } else {
                free(full);
            }
        }
        closedir(d);
    } else {
        files = (char**)malloc(sizeof(char*));
        if (!files) return;
        files[0] = strdup(file_path);
        files_cnt = 1;
        files_cap = 1;
    }

    for (size_t i=0;i<files_cnt;i++) {
        EntireFile ef = read_entire_file_into_memory(files[i]);
        if (ef.len < 1) {
            fprintf(stderr, "Skipping empty/too-small file: %s\n", files[i]);
            if (ef.contents) free(ef.contents);
            continue;
        }

        /* create simple provider for this file. NOTE: provider does NOT copy data,
           so we pass the buffer pointer directly (contents must remain valid). */
        simple_fdp* p = simple_fdp_create((const uint8_t*)ef.contents, ef.len);
        if (!p) {
            fprintf(stderr, "Failed to create provider for %s\n", files[i]);
            free(ef.contents);
            continue;
        }
        global_provider = p;

        /* run all tests */
        for (size_t t=0; t<tests_count; ++t) {
            if (tests[t]) tests[t]();
        }

        /* cleanup */
        simple_fdp_destroy(p);
        global_provider = NULL;
        free(ef.contents);
    }

    /* free files array */
    for (size_t i=0;i<files_cnt;i++) free(files[i]);
    free(files);
}