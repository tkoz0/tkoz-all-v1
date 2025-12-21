#!/bin/bash
source "$(dirname $0)/bash_common.sh"
check_build_type $1
cmake --build "$(dirname $0)/../builds/$1" --parallel $(nproc) --verbose
