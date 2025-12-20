#!/bin/bash
cmake --build "$(dirname $0)/../builds/Release" --parallel $(nproc) --verbose
