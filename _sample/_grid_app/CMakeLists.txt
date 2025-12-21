add_executable(_grid_app grid.cpp)

target_link_libraries(_grid_app
    PRIVATE
        tkoz_options_common
        _template_lib
)
