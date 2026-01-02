#!/bin/bash
# runs clang-tidy limited checks on provided files (still slow)
# uses the .clang-tidy file
root_dir=$(realpath "$(dirname "$0")/..")
clang-tidy -p "$root_dir" "$@"
# note: clang-tidy should only be given .cpp files
# note: use $(git ls-files '*.cpp') to run on all
# todo: use xargs to parallelize it
