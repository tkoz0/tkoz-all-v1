#!/bin/bash
# removes generated coverage info files
# cov_cleanup.sh <GCC|Clang>
source "$(dirname "$0")/bash_common.sh"
check_compiler_type "$1"
root_dir=$(realpath "$(dirname "$0")/..")
per_file_cmd='echo '\''removing:'\'' "$1" && rm -f "$1"'
find "$root_dir/builds/Coverage-$1" \
    \( \
        -name '*.gcda' -o \
        -name '*.profraw' -o \
        -name '*.profdata' -o \
        -name 'coverage.info' \
    \) \
    -exec sh -c "$per_file_cmd" _ {} \;
# maybe prefer the following which would be faster
#find "$root_dir/builds/Coverage-$1" \
#    \( \
#        -name '*.gcda' -o \
#        -name '*.profraw' -o \
#        -name '*.profdata' -o \
#        -name 'coverage.info' \
#    \) \
#    -print -delete
