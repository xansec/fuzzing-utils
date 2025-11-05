#ifndef FUZZING_UTILS_H
#define FUZZING_UTILS_H

#include "entire_file_utils.h"
#include "fuzzed_data_provider.h"

extern fuzzed_data_provider* provider;

// Bunch of variable replacement
#define FUZZ_TEST(test_group, test_name) \
    void test_group##_##test_name##_FuzzTest(); \
    struct test_group##_##test_name##_FuzzTestRunner { \
        test_group##_##test_name##_FuzzTestRunner() { RegisterFuzzTest(test_group##_##test_name##_FuzzTest); } \
    } test_group##_##test_name##_instance; \
    void test_group##_##test_name##_FuzzTest()

typedef void (*FuzzTestFunc)();
void RegisterFuzzTest(FuzzTestFunc func);
void RunFuzzTests(const char* file_path);

#endif // FUZZING_UTILS_H
