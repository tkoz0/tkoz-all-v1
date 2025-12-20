#!/bin/bash
cmake -S "$(dirname $0)/.." -B "$(dirname $0)/../builds/Release" -DCMAKE_BUILD_TYPE=Release
