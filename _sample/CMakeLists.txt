add_subdirectory(_template_lib) # header only
add_subdirectory(_math_lib) # compiled code (depends on compiled library)
add_subdirectory(_factor_app) # executable
add_subdirectory(_math_tests) # unit tests
add_subdirectory(_grid_app) # compiled code (depends on header only library)
#if (CMAKE_BUILD_TYPE MATCHES "Coverage|Debug|RelWithDebInfo")
add_subdirectory(_test_run) # for a static registry unit tests library
#endif()
