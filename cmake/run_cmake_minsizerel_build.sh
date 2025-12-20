#!/bin/bash
cmake --build "$(dirname $0)/../builds/MinSizeRel" --parallel $(nproc) --verbose
