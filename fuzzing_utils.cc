#include <vector>
#include <iostream>
#include <filesystem>
#include "fuzzing_utils.h"

static std::vector<FuzzTestFunc> &fuzz_tests() {
    static std::vector<FuzzTestFunc> _instance;
    return _instance;
}

void RegisterFuzzTest(FuzzTestFunc func) {
    fuzz_tests().push_back(func);
}

void RunFuzzTests(const char* file_path) {
    if (fuzz_tests().empty()) {
        std::cerr << "Error: no fuzz tests registered.\n";
        return;
    }
    
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
        std::sort(files.begin(), files.end());
    } else if (std::filesystem::is_regular_file(input_path)) {
        files.push_back(input_path);
    }

    for (const auto &testcase : files) {
        EntireFile file = read_entire_file_into_memory(testcase.string().c_str());

        if (file.contents == nullptr) {
            std::cerr << "Warning: could not read file: " << testcase << "\n";
            continue;
        }

        if (file.len < 2) {
            std::cerr << "File is too small." << std::endl;
            free(file.contents);
            return;
        }

        const unsigned char* data = reinterpret_cast<const unsigned char*>(file.contents);
        size_t len = file.len;

        // Init provider
        fuzzed_data_provider fdp(data, len);

        // Run single fuzz test based on data index
        if (!fuzz_tests().empty()) {
            size_t index = static_cast<size_t>(fdp.ConsumeIntegral<uint64_t>() % fuzz_tests().size());
            fuzz_tests()[index](&fdp);
        }
        free(file.contents);
    }
}
