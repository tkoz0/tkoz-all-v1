#!/bin/bash
source "$(dirname $0)/bash_common.sh"
check_build_type $1
check_compiler_type $2
ctest --test-dir "$(dirname $0)/../builds/$1-$2" --parallel $(nproc) --output-on-failure
