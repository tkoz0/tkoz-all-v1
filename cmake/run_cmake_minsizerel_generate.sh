#!/bin/bash
cmake -S "$(dirname $0)/.." -B "$(dirname $0)/../builds/MinSizeRel" -DCMAKE_BUILD_TYPE=MinSizeRel
