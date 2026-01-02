#!/bin/bash
# runs clang-tidy full checks on provided files (slower)
root_dir=$(realpath "$(dirname "$0")/..")
clang-tidy -p "$root_dir" \
    -checks='*,clang-analyzer-*' \
    -header-filter='^(src|inc)/' \
    -warnings-as-errors='*' \
    "$@"
# note: clang-tidy should only be given .cpp files
# note: use $(git ls-files '*.cpp') to run on all
# todo: use xargs to parallelize it
