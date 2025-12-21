add_executable(_test_run run.cpp)

target_compile_definitions(_test_run PRIVATE TEST)

target_link_libraries(_test_run
    PRIVATE
        tkoz_options_common
        tkoz-srtest

        # Ensure all libraries with tests are linked fully so the static test
        # registration works even if no symbols are actually referenced.

        -Wl,--whole-archive

        _math_lib

        -Wl,--no-whole-archive
)
