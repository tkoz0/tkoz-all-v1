add_library(tkoz-srtest INTERFACE)

target_include_directories(tkoz-srtest INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_definitions(tkoz-srtest INTERFACE TKOZ_SRTEST_SOURCE_ROOT="${CMAKE_SOURCE_DIR}/")

target_link_libraries(tkoz-srtest
    INTERFACE
        tkoz_options_common
)
