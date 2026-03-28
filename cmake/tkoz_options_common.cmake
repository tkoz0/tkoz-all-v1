add_library(tkoz_options_common INTERFACE)
target_compile_features(tkoz_options_common INTERFACE cxx_std_23)

# Always enable most warnings, if they were intended then they can be suppressed
# For release builds, we should also use -Werror
target_compile_options(tkoz_options_common INTERFACE
    -Wall # baseline
    -Wextra # more warnings
    -Wpedantic # for anything violating ISO C++
    -Wconversion # implicit type conversions
    -Wshadow # variable hides another of same name
    -Wformat=2 # strongly check printf/scanf format strings
    -Wnull-dereference # obvious null pointer dereference
    -Wimplicit-fallthrough # require [[fallthrough]] attribute
    -Wsign-conversion # conversion between signed and unsigned
    -Wdouble-promotion # converting float to double
    -Wunreachable-code # detect code never used
    -Wuninitialized # warn on used variables that might be uninitialized
    -Wstrict-overflow=2 # detect assumptions about integer overflow
    -Woverflow # detect signed overflow in constants
    -Wmissing-noreturn # warn about missing [[noreturn]]
    -Wunused-parameter # warn about unused function parameters
    -Wunused-function # warn about static function never called
    -Wunused-variable # warn about unused local variables
    -Woverloaded-virtual # warn when hiding with another signature
    -Wdeprecated # warn when using deprecated function or types
    -Wnon-virtual-dtor # warn if there are virtual functions but no virtual dtor
    -Wsign-promo # warn when smaller signed type is promoted to int
    -Wctad-maybe-unsupported # warn if ctad is potentially unsafe
    -Walloca # warn about large stack allocations
    -Wstrict-aliasing # warn about undefined behavior from pointer type pruning

    -frtti # enable run time type information

    #-Wfloat-equal # warn on equality comparison of floating point
)

# Options specific to Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(tkoz_options_common INTERFACE
        -Wshorten-64-to-32 # conversion to smaller integer type
        -Wunused-private-field # warn about unused field in class/struct
        -Wweak-vtables # warn if a class may get multiple weak vtables
        -Wstring-conversion # warn when converting between char* and std::string

        -Wno-c2y-extensions # silence warning about __COUNTER__
    )
endif()

# Options specific to GCC
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(tkoz_options_common INTERFACE
        -Wmaybe-uninitialized # warn on used variables that might be uninitialized
    )
endif()

# Note: address/undefined sanitizers may cause issues for intentional low level code

# Debug
target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:Debug>: -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer >)
target_link_options(tkoz_options_common INTERFACE
    $<$<CONFIG:Debug>: -fsanitize=address,undefined >)

# RelWithDebInfo
target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:RelWithDebInfo>: -g -O2 -fno-omit-frame-pointer >)

# MinSizeRel
target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:MinSizeRel>: -Os -Werror >)

# Release
target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:Release>: -O3 -Werror >)

# Coverage
target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:Coverage>: -g -O0 -fno-omit-frame-pointer >)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(tkoz_options_common INTERFACE $<$<CONFIG:Coverage>: --coverage >)
    target_link_options(tkoz_options_common INTERFACE $<$<CONFIG:Coverage>: --coverage >)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(tkoz_options_common INTERFACE $<$<CONFIG:Coverage>: -fprofile-instr-generate -fcoverage-mapping >)
    target_link_options(tkoz_options_common INTERFACE $<$<CONFIG:Coverage>: -fprofile-instr-generate >)
endif()
