#!/bin/bash
source "$(dirname $0)/bash_common.sh"
check_build_type $1
cmake -S "$(dirname $0)/.." -B "$(dirname $0)/../builds/$1" -DCMAKE_BUILD_TYPE=$1
