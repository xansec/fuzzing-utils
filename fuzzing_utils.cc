#include <vector>
#include <iostream>
#include <filesystem>
#include "fuzzing_utils.h"

// Global variables
fuzzed_data_provider* provider = nullptr;

static std::vector<FuzzTestFunc> &fuzz_tests() {
    static std::vector<FuzzTestFunc> _instance;
    return _instance;
}

void RegisterFuzzTest(FuzzTestFunc func) {
    fuzz_tests().push_back(func);
}

void RunFuzzTests(const char* file_path) {
    std::filesystem::path input_path(file_path);
    if (!std::filesystem::exists(input_path)) {
        std::cerr << "Error: " << file_path << " does not exist.\n" << std::endl;
        return;
    }

    std::vector<std::filesystem::path> files;
    if (std::filesystem::is_directory(input_path)) {
        for (const auto &entry : std::filesystem::directory_iterator(input_path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path());
            }
        }
    } else if (std::filesystem::is_regular_file(input_path)) {
        files.push_back(input_path);
    }

    for (const auto &testcase : files) {
        const char* testcase_ptr = testcase.string().c_str();
        EntireFile file = read_entire_file_into_memory(testcase.string().c_str());

        const unsigned char* data = reinterpret_cast<const unsigned char*>(file.contents);
        size_t len = file.len;

        // Init provider
        fuzzed_data_provider fdp(data, len);
        provider = &fdp;

        // Run single fuzz test based on data index
        if (len > 0) {
            fuzz_tests().at((data[0] % len));
        }
        free(file.contents);
    }

    provider = nullptr;  // Reset provider; this whole function should only run once anyways
}
