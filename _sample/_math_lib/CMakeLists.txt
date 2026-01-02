add_library(_math_lib)

# CONFIGURE_DEPENDS is necessary so CMake reruns when adding/deleting files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

target_sources(_math_lib PRIVATE ${SOURCES})

target_include_directories(_math_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

target_link_libraries(_math_lib
    PRIVATE
        tkoz_options_common
        _template_lib
        tkoz-srtest
)
