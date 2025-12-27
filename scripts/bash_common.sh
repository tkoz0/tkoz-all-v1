#!/bin/bash

# usage: check_build_type $1
# exit status is 0 when valid
# when build type is invalid, prints message and exits
check_build_type() {
    local build_types=(Debug Release RelWithDebInfo MinSizeRel Coverage)
    local is_valid=1
    for build_type in "${build_types[@]}"; do
        if [[ "$1" == "$build_type" ]]; then
            is_valid=0
            break
        fi
    done
    if [[ $is_valid -ne 0 ]]; then
        echo 'not a valid build type'
        echo 'valid build types are:' "${build_types[@]}"
        exit 1
    fi
    return $is_valid
}

# usage: check_compiler_type $1
# exit status is 0 when valid
# when compiler type is invalid, prints message and exits
check_compiler_type() {
    local compiler_types=(GCC Clang)
    local is_valid=1
    for compiler_type in "${compiler_types[@]}"; do
        if [[ "$1" == "$compiler_type" ]]; then
            is_valid=0
            break
        fi
    done
    if [[ $is_valid -ne 0 ]]; then
        echo 'not a valid compiler type'
        echo 'valid compiler types are:' "${compiler_types[@]}"
        exit 1
    fi
    return $is_valid
}

# usage: toolchain_file $1
# prints the file name to stdout, exits if invalid
toolchain_file() {
    if [[ "$1" == "GCC" ]]; then
        echo 'gcc.cmake'
    elif [[ "$1" == "Clang" ]]; then
        echo 'clang.cmake'
    else
        echo 'not a valid compiler type'
        exit 1
    fi
}
