add_library(tkoz-_testlib1 source/test1.cpp)

target_include_directories(tkoz-_testlib1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(tkoz-_testlib1
    INTERFACE
        tkoz_options_common
)
