#!/bin/bash
cmake -S "$(dirname $0)/.." -B "$(dirname $0)/../builds/Debug" -DCMAKE_BUILD_TYPE=Debug
