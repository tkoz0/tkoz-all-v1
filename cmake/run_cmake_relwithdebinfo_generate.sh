#!/bin/bash
cmake -S "$(dirname $0)/.." -B "$(dirname $0)/../builds/RelWithDebInfo" -DCMAKE_BUILD_TYPE=RelWithDebInfo
