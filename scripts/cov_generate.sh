#!/bin/bash
source "$(dirname "$0")/bash_common.sh"
check_compiler_type "$1"
build_dir="$(dirname "$0")/../builds/Coverage-$1"
root_dir=$(realpath "$(dirname "$0")/..")

if [[ "$1" == 'GCC' ]]; then
    echo 'generating coverage reports for gcc compiled projects'
    gcda_files=$(find "$build_dir" -name '*.gcda' 2>/dev/null || true)
    lcov \
        --capture \
        --directory="$build_dir" \
        --output-file "$build_dir/coverage.info" \
        --ignore-errors mismatch
    lcov \
        --remove "$build_dir/coverage.info" \
        '/usr/*' \
        --output-file "$build_dir/coverage.info"
    genhtml "$build_dir/coverage.info" \
        --output-directory "$build_dir/coverage-report-gcc"

elif [[ "$1" == 'Clang' ]]; then
    echo 'generating coverage reports for clang compiled projects'
    profraw_files=$(find "$build_dir" -name '*.profraw' 2>/dev/null || true)
    merged_profdata="$build_dir/coverage.profdata"
    llvm-profdata merge -sparse "$profraw_files" -o "$merged_profdata"

    # TODO need a test runner executable that links everything
    # running to generate data requires setting LLVM_PROFILE_FILE
    echo 'generating coverage reports for clang is not fully supported yet'
    #exit 1
    test_runner="$build_dir/_sample/_test_run/_test_run"
    llvm-cov show "$test_runner" \
        -instr-profile="$merged_profdata" \
        --format=html \
        --output-dir="$build_dir/coverage-report-clang" \
        --ignore-filename-regex='/usr/include'

else
    echo 'unexpected compiler type'
    exit 1
fi
