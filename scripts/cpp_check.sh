#!/bin/bash
root_dir=$(realpath "$(dirname "$0")/..")
cppcheck \
    --project="$root_dir/compile_commands.json" \
    --std=c++23 \
    --enable=warning,performance,style \
    --inconclusive \
    --inline-suppr
