add_executable(_math_tests main.cpp)

target_link_libraries(_math_tests
    PRIVATE
        tkoz_options_common
        _math_lib
)

# CTest unit tests
add_test(NAME _math_tests_small COMMAND _math_tests small)
add_test(NAME _math_tests_large COMMAND _math_tests large)
add_test(NAME _math_tests_other COMMAND _math_tests other)
