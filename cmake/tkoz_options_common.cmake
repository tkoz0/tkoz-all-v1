add_library(tkoz_options_common INTERFACE)
target_compile_features(tkoz_options_common INTERFACE cxx_std_23)

# Always enable all warnings, if they were intended then they can be suppressed
# For release builds, we should also use -Werror
target_compile_options(tkoz_options_common INTERFACE
    -Wall -Wextra -Wpedantic -frtti)

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

# Define TEST macro for Debug, RelWithDebInfo, and Coverage
# This allows tests to be built (they are defined between #if TEST ... #endif)
target_compile_definitions(tkoz_options_common INTERFACE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>,$<CONFIG:Coverage>>: TEST >)
