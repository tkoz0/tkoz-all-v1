add_executable(tkoz-ff-test-runner main.cpp)

target_link_libraries(tkoz-ff-test-runner
    PRIVATE
        tkoz_options_common
        tkoz-srtest

        # Ensure all libraries with tests are linked fully so the static test
        # registration works even if no symbols are actually referenced.

        -Wl,--whole-archive

        tkoz-ff-core-tests

        -Wl,--no-whole-archive
)
