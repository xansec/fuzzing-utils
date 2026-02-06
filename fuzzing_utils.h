#ifndef FUZZING_UTILS_H
#define FUZZING_UTILS_H

#include "entire_file_utils.h"
#include "fuzzed_data_provider.h"

// Bunch of variable replacement
// Also now we implicitly pass a pointer to fdp when expanding the macro
#define FUZZ_TEST(test_group, test_name) \
    void test_group##_##test_name##_FuzzTest(fuzzed_data_provider* _fdp); \
    struct test_group##_##test_name##_FuzzTestRunner { \
        test_group##_##test_name##_FuzzTestRunner() { RegisterFuzzTest(test_group##_##test_name##_FuzzTest); } \
    } test_group##_##test_name##_instance; \
    void test_group##_##test_name##_FuzzTest(fuzzed_data_provider* _fdp)

#define INIT_FUZZ_TEST fuzzed_data_provider& provider = *_fdp;

typedef void (*FuzzTestFunc)(fuzzed_data_provider*);
void RegisterFuzzTest(FuzzTestFunc func);
void RunFuzzTests(const char* file_path);

#endif // FUZZING_UTILS_H
