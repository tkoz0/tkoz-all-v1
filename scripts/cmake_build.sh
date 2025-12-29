#!/bin/bash
source "$(dirname "$0")/bash_common.sh"
check_build_type "$1"
check_compiler_type "$2"
root_dir=$(realpath "$(dirname "$0")/..")
cmake --build "$root_dir/builds/$1-$2" --parallel $(nproc) --verbose
