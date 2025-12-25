add_library(tkoz_options_common INTERFACE)
target_compile_features(tkoz_options_common INTERFACE cxx_std_23)

# Always enable all warnings, if they were intended then they can be suppressed
target_compile_options(tkoz_options_common INTERFACE
    -Wall -Wextra -Wpedantic)

# Note: address/undefined sanitizers may cause issues for intentional low level code
target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:Debug>: -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer >)
target_link_options(tkoz_options_common INTERFACE
    $<$<CONFIG:Debug>: -fsanitize=address,undefined >)

target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:RelWithDebInfo>: -g -O2 >)

target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:MinSizeRel>: -Os -Werror >)

target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:Release>: -O3 -Werror >)

target_compile_options(tkoz_options_common INTERFACE
    $<$<CONFIG:Coverage>: -g -O0 >)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(tkoz_options_common INTERFACE $<$<CONFIG:Coverage>: --coverage >)
    target_link_options(tkoz_options_common INTERFACE $<$<CONFIG:Coverage>: --coverage >)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(tkoz_options_common INTERFACE $<$<CONFIG:Coverage>: -fprofile-instr-generate -fcoverage-mapping >)
    target_link_options(tkoz_options_common INTERFACE $<$<CONFIG:Coverage>: -fprofile-instr-generate >)
endif()

# Define TEST macro for Debug, RelWithDebInfo, and Coverage
target_compile_definitions(tkoz_options_common INTERFACE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>,$<CONFIG:Coverage>>: TEST >)
