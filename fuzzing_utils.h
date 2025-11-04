#ifndef FUZZING_UTILS_H
#define FUZZING_UTILS_H

#include <stddef.h>
#include "simple_fdp.h"

/* Global provider pointer used by tests */
extern simple_fdp* global_provider;

/* INIT_FUZZ_TEST provides a local 'provider' pointer and checks initialization. */
#define INIT_FUZZ_TEST \
    simple_fdp* provider = global_provider; \
    if (!global_provider) { \
        fprintf(stderr, "Error: simple_fdp provider not initialized!\n"); \
        return; \
    }

/* FUZZ_TEST macro for declaring and auto-registering a test (GCC/Clang constructor style). */
#ifdef __GNUC__
#define FUZZ_TEST(group, name) \
    void group##_##name##_FuzzTest(void); \
    static void group##_##name##_registrar(void) __attribute__((constructor)); \
    static void group##_##name##_registrar(void) { RegisterFuzzTest(group##_##name##_FuzzTest); } \
    void group##_##name##_FuzzTest(void)
#else
#warning "Auto-registration not supported on this compiler. Call RegisterFuzzTest() manually."
#define FUZZ_TEST(group, name) \
    void group##_##name##_FuzzTest(void); \
    void group##_##name##_FuzzTest(void)
#endif

typedef void (*FuzzTestFunc)(void);
void RegisterFuzzTest(FuzzTestFunc func);
void RunFuzzTests(const char* file_path);

#endif /* FUZZING_UTILS_H */