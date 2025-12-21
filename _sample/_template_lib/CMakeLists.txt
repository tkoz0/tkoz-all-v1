add_library(_template_lib INTERFACE)

target_include_directories(_template_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(_template_lib
    INTERFACE
        tkoz_options_common
)
