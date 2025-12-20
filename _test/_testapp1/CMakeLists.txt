add_executable(tkoz-_testapp1 source/app1.cpp)

target_link_libraries(tkoz-_testapp1
    INTERFACE
        tkoz_options_common
    PRIVATE
        tkoz-_testlib1
        tkoz-_testlib2
)
