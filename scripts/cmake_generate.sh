#!/bin/bash
source "$(dirname "$0")/bash_common.sh"
check_build_type "$1"
check_compiler_type "$2"
root_dir=$(realpath "$(dirname "$0")/..")
cmake -S "$root_dir" -B "$root_dir/builds/$1-$2" \
    -DCMAKE_BUILD_TYPE=$1 \
    -DCMAKE_TOOLCHAIN_FILE="$root_dir/cmake/toolchains/$(toolchain_file "$2")"
ln -sf "$root_dir/builds/$1-$2/compile_commands.json" "$root_dir/compile_commands.json"
