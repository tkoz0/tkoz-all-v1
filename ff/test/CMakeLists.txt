add_executable(tkoz-ff-test main.cpp)

target_compile_definitions(tkoz-ff-test PRIVATE TEST)

target_link_libraries(tkoz-ff-test
    PRIVATE
        tkoz_options_common
        tkoz-srtest

        # Ensure all libraries with tests are linked fully so the static test
        # registration works even if no symbols are actually referenced.

        -Wl,--whole-archive

        tkoz-ff-core
        tkoz-ff-json
        tkoz-ff-vars

        -Wl,--no-whole-archive
)
