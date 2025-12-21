#!/bin/bash

# usage: check_build_type $1
# exit status is 0 when valid
# when build type is invalid, prints message and exits
check_build_type() {
    local build_types=(Debug Release RelWithDebInfo MinSizeRel)
    local is_valid=1
    for build_type in "${build_types[@]}"; do
        if [[ "$1" == "$build_type" ]]; then
            is_valid=0
            break
        fi
    done
    if [[ $is_valid -ne 0 ]]; then
        echo not a valid build type
        echo valid build types are: "${build_types[@]}"
        exit 1
    fi
    return $is_valid
}
