#!/bin/bash
source "$(dirname $0)/bash_common.sh"
check_build_type $1
ctest --test-dir "$(dirname $0)/../builds/$1" --parallel $(nproc) --output-on-failure
