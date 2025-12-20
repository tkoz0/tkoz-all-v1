#!/bin/bash
cmake --build "$(dirname $0)/../builds/RelWithDebInfo" --parallel $(nproc) --verbose
