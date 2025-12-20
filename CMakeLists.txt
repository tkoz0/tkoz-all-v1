cmake_minimum_required(VERSION 4.0)
project(tkoz-all LANGUAGES CXX)

# This gives a JSON file that is useful for an IDE
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use standard C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add in interface link targets for common build options
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(tkoz_options_common)

# Set default build type and available options
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

# Projects in _test are just for testing CMake setup and similar
add_subdirectory(_test)
