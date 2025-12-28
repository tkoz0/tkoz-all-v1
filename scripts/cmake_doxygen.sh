#!/bin/bash
source "$(dirname "$0")/bash_common.sh"
check_build_type "$1"
check_compiler_type "$2"
cmake --build "$(dirname "$0")/../builds/$1-$2" --verbose --target docs
