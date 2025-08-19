#include <vector>
#include <iostream>
#include <filesystem>
#include "fuzzing_utils.h"

// Global variables
fuzzed_data_provider* global_provider = nullptr;
std::vector<FuzzTestFunc> fuzz_tests;

void RegisterFuzzTest(FuzzTestFunc func) {
    fuzz_tests.push_back(func);
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
        if (!file.contents) {
            std::cerr << "Failed to read file: " << testcase.string().c_str() << std::endl;
            return;
        }
        if (file.len < 2) {
            std::cerr << "File is too small." << std::endl;
            return;
        }

        const unsigned char* data = reinterpret_cast<const unsigned char*>(file.contents);
        size_t len = file.len;

        // Create and set up FDP instance
        fuzzed_data_provider provider(data, len);
        global_provider = &provider;  // Make the provider available to all tests without reinitializing it on every pass

        // Run all registered fuzz tests using the same provider
        for (auto& fuzz_test : fuzz_tests) {
            fuzz_test();
        }
    }

    global_provider = nullptr;  // Reset provider; this whole function should only run once anyways
}
