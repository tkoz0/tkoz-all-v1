add_executable(tkoz-_testapp2 source/app2.cpp)

target_link_libraries(tkoz-_testapp2
    INTERFACE
        tkoz_options_common
    PRIVATE
        tkoz-_testlib1
        tkoz-_testlib2
)
