add_library(tkoz-_testlib2 INTERFACE)

target_include_directories(tkoz-_testlib2 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(tkoz-_testlib2
    INTERFACE
        tkoz_options_common
)
