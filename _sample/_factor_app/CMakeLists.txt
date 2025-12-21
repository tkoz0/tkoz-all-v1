add_executable(_factor_app main.cpp)

target_link_libraries(_factor_app
    PRIVATE
        tkoz_options_common
        _math_lib
)
