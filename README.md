# Fuzzing Utils

Header only files for creating `FUZZ_TEST()` macros to use along-side Google Test-style `TEST()` macros.

## Usage

Include the `fuzzing_utils.h` header in your test files:

```cpp
#include "fuzzing_utils.h"
```

Then, define your fuzz test cases using the `FUZZ_TEST()` macro:

```cpp
FUZZ_TEST(MyTest, FuzzMyTestFunction) {
    INIT_FUZZ_TEST;
    // initialize variables with FuzzedDataProvider "provider"
    // call your test function with fuzzed data
}
```

For example, if you have a test that looks like:

```cpp
TEST(FooTest, TestFoo) {
  test_foo("hello", 42);
}
```

This allows you to create a FUZZ_TEST fixture that looks like:

```cpp
FUZZ_TEST(FooTest, FuzzTestFoo) {
    INIT_FUZZ_TEST;
    std::string str = provider.ConsumeRandomLengthString();
    int num = provider.ConsumeIntegral<int>();
    test_foo(str, num);
}
```

Finally, at the bottom of your test file, you need to define your `main()` function to conditionally run the fuzz tests on the presence of a command line argument (file input, standard for most fuzzers):

```cpp
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    if (argc > 1) {
        RunFuzzTests(argv[1]);
        return 0;
    }

    return RUN_ALL_TESTS(); 
}
```