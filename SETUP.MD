# development instructions

First generate a CMake build area and build using a pair of scripts in `/cmake`.
For example, with a debug build you would run the following:
`./cmake/run_cmake_generate.sh Debug && ./cmake/run_cmake_build.sh Debug`
The build types available are: Debug, Release, RelWithDebInfo, MinSizeRel.
Additionally, tests with CTest can be run with
`./cmake/run_cmake_ctest.sh Debug`

# creating a library (compiled)

Create a directory for the library in the appropriate place. Then it should get
a `CMakeLists.txt` like the one below and 2 subdirectories named `source` and
`include`. Inside include we need the full path which for many of my things is
going to start with `tkoz/`. Tests are defined inside the `.cpp` files and are
found by a test runner that links the library (via static registration).

```cmake
add_library(library-name)

# Recursively find .cpp files in source.
# CMake should rerun when a .cpp file is created or removed.
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS source/*.cpp)
target_sources(library-name PRIVATE ${SOURCES})

target_include_directories(library-name
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(library-name
    PRIVATE
        tkoz_options_common # Commonly used compile/linking options
        tkoz-srtest         # The testing library I created for now
        other-library       # Link other libraries
)
```

# creating a library (header only)

Make a directory for it, and inside it just needs a `CMakeLists.txt` like the
one below and a `include` directory which contains the full paths inside.
Testing will require a separate project which links this library. See the
appropriate section for that.

```cmake
add_library(header-library INTERFACE)

target_include_directories(header-library
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(header-library
    INTERFACE
        tkoz_options_common
)
```

# creating tests

For compiled libraries, the tests go at the bottom of the `.cpp` files after
the actual library code. For header only libraries, they get a separate project
that links the header library and defines tests in `.cpp` files. Create a
`#if TEST` block for conditional compilation and define tests similar to below.

```cpp
#if TEST

TEST_CREATE(testName)
{
    int a = 5;
    int b = 10;
    TEST_REQUIRE(a+b == 15);
}

#endif // TEST
```

The ideas/concepts for a test library may still be work in progress and could
change. The goal is to have macros that can be changed later without changing
the test code and to be able to support dynamic test discovery in the future.

# creating an executable (for an application)

These get a `CMakeLists.txt` in their directory and a `source` directory. If
they have headers, they go inside `source` as well. Applications do not have
a public interface that other projects can link to. They also should not have
test cases.

```cmake
add_executable(app-name)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS source/*.cpp)
target_sources(app-name PRIVATE ${SOURCES})
target_link_libraries(app-name
    PRIVATE
        tkoz_options_common
        library-name
)
```

# creating an executable (for CTest unit tests)

This is not recommended for future scalability. Create an executable with a
`CMakeLists.txt` and a `source` directory. The executable should take command
line arguments for running its tests. Then for CTest, use the `add_test()`
function in `CMakeLists.txt` to add tests which each have a name and a command
which should pass arguments to the executable.

```cmake
add_executable(library-test-app)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS source/*.cpp)

target_link_libraries(library-test-app
    PRIVATE
        tkoz_options_common
        library-name
)

add_test(NAME library-test-app-test-1 COMMAND library-test-app test1)
add_test(NAME library-test-app-test-2 COMMAND library-test-app test2)
add_test(NAME library-test-app-test-3 COMMAND library-test-app test3)
```

# creating an executable (for statically linked tests)

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
        library-1
        library-2

        -Wl,--no-whole-archive
)
```

```cpp
// The header additionally defines the main function with this macro.
#define TKOZ_SRTEST_MAIN
#include <tkoz/SRTest.hpp>
```

Currently this test runner is minimal and needs more features.
