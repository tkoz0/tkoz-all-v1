# development instructions

First generate a CMake build area and build using a pair of scripts in
`/scripts`. For example, with a debug build using GCC you would run the
following:
```bash
./scripts/cmake_generate.sh Debug GCC
./scripts/cmake_build.sh Debug GCC
```
The build types available are: Debug, Release, RelWithDebInfo, MinSizeRel, and
Coverage. Additionally, tests with CTest can be run with
`./scripts/cmake_ctest.sh Debug GCC`

## creating a library (compiled)

Create a directory for the library in the appropriate place. Then it should get
a `CMakeLists.txt` like the one below and 2 subdirectories named `src` and
`inc`. Inside `inc` we need the full path which for many of my things is
going to start with `tkoz/`. Additionally, create a library of the same name
but with `-tests` at the end, and a `tests` directory for unit tests. This
allows separating unit tests from production code.

```cmake
add_library(library-name)
add_library(library-name-tests)

# Recursively find .cpp files in source.
# CMake should rerun when a .cpp file is created or removed.
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
target_sources(library-name PRIVATE ${SOURCES})

file(GLOB_RECURSE SOURCES_TESTS CONFIGURE_DEPENDS tests/*.cpp)
target_sources(library-name PRIVATE ${SOURCES_TESTS})

target_include_directories(library-name
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

target_link_libraries(library-name
    PRIVATE
        tkoz_options_common # Commonly used compile/linking options
        other-library       # Link other libraries
)

target_link_libraries(library-name-tests
    PRIVATE
        tkoz-srtest # Link the test library, needed for writing tests
        tkoz_options_common
        other-library
)
```

## creating a library (header only)

Make a directory for it, and inside it just needs a `CMakeLists.txt` like the
one below and a `inc` directory which contains the full paths inside.
Testing will require a separate project which links this library. See the
appropriate section for that.

```cmake
add_library(header-library INTERFACE)

target_include_directories(header-library
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/inc")

target_link_libraries(header-library
    INTERFACE
        tkoz_options_common
)
```

## creating tests

For a library named `library-name` (either header only or compiled), create a
compiled library named `library-name-tests` which links `library-name`. Then
its `.cpp` files contain tests using the `TEST_*` macros defined in
`"tkoz/SRTest.hpp"`.

```cpp
TEST_CREATE(testName)
{
    int a = 5;
    int b = 10;
    TEST_REQUIRE(a+b == 15);
}
```

The ideas/concepts for a test library may still be work in progress and could
change. The goal is to have macros that can be changed later without changing
the test code and to be able to support dynamic test discovery in the future.

## creating an executable (for an application)

These get a `CMakeLists.txt` in their directory and a `src` directory. If
they have headers, they go inside `src` as well. Applications do not have
a public interface that other projects can link to. They also should not have
test cases.

```cmake
add_executable(app-name)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
target_sources(app-name PRIVATE ${SOURCES})
target_link_libraries(app-name
    PRIVATE
        tkoz_options_common
        library-name
)
```

## creating an executable (for CTest unit tests)

This is not recommended for future scalability. Create an executable with a
`CMakeLists.txt` and a `src` directory. The executable should take command
line arguments for running its tests. Then for CTest, use the `add_test()`
function in `CMakeLists.txt` to add tests which each have a name and a command
which should pass arguments to the executable.

```cmake
add_executable(library-test-app)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

target_link_libraries(library-test-app
    PRIVATE
        tkoz_options_common
        library-name
)

add_test(NAME library-test-app-test-1 COMMAND library-test-app test1)
add_test(NAME library-test-app-test-2 COMMAND library-test-app test2)
add_test(NAME library-test-app-test-3 COMMAND library-test-app test3)
```

## creating an executable (for statically linked tests)

This is done by making a simple executable to use the main function from the
`tkoz-srtest` library. Tests are statically registered from the libraries
linked to this executable. Create a `CMakeLists.txt` like below and a small
`main.cpp` as follows.

```cmake
add_executable(test-runner main.cpp)

# Define the macro used for conditionally compiling the tests.
target_compile_definitions(test-runner PRIVATE TEST)

target_link_libraries(_test_run
    PRIVATE
        tkoz_options_common
        tkoz-srtest

        # Ensure all libraries with tests are linked fully so the static test
        # registration works even if no symbols are actually referenced.
        -Wl,--whole-archive

        # List the libraries here, which are forced to be linked even
        # though the test runner references none of their symbols.
        library-1-tests
        library-2-tests

        -Wl,--no-whole-archive
)
```

```cpp
// Just a single line to include the definition of main()
// to be used and related functionality it depends on.
#include <tkoz/SRTestMain.hpp>
```

Currently this test runner is minimal and needs more features.

## coverage reports

These require a build with the instrumentation for coverage reports so
"Coverage" is a separate build type, with either GCC and Clang. Once a build
for coverage is available, the required executables must be run to output the
coverage data which is used to generate reports.

TODO setup a system for running the required executables to generate coverage
data easily. Additionally for Clang, an executable is needed to link all
libraries used in the coverage report (see `scripts/cov_generate.sh`). It may
be fine to make a script (`scripts/cov_runner.sh`) to generate the coverage
data, which must be serial for GCC and can be parallel for Clang.

### gcc

GCC builds place coverage (`*.gcda` files) next to the compiled object files.
Coverage data is accumulated by multiple runs, but they cannot be in parallel
since they all try to write to the same file. The paths used by instrumented
executables from GCC are fixed and the coverage system is not suitable for large
scale parallelism. Coverage data is accumulated with `lcov` and reports are
generated with `genhtml`.

### clang

By default, coverage data is written to `default.profraw` in the current working
directory (existing files are overwritten). This file location can be customized
with the `LLVM_PROFILE_FILE` environment variable. This coverage system can be
parallelized at large scale. Data from coverage runs are merged with
`llvm-profdata` and the reports are generated with `llvm-cov`.

## documentation

These can be created by running the `cmake_clang_doc.sh` or `cmake_doxygen.sh`
scripts. They generate documentation in the specified build area. Note that
Doxygen is fast and clang-doc is slow. Also, Doxygen produces nicer looking
documentation pages from comments, clang-doc can produce more accurate
documentation because it uses the clang compiler.
