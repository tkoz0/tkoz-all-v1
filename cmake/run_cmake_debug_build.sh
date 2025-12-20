#!/bin/bash
cmake --build "$(dirname $0)/../builds/Debug" --parallel $(nproc) --verbose
