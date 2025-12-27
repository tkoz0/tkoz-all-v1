#!/bin/bash
source "$(dirname "$0")/bash_common.sh"
check_build_type "$1"
check_compiler_type "$2"
cmake -S "$(dirname "$0")/.." -B "$(dirname "$0")/../builds/$1-$2" \
    -DCMAKE_BUILD_TYPE=$1 \
    -DCMAKE_TOOLCHAIN_FILE="$(dirname "$0")/../cmake/toolchains/$(toolchain_file "$2")"
