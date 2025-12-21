# development instructions

First generate a CMake build area and build using a pair of scripts in `/cmake`.
For example, with a debug build you would run the following:
`./cmake/run_cmake_generate.sh Debug && ./cmake/run_cmake_build.sh Debug`
The build types available are: Debug, Release, RelWithDebInfo, MinSizeRel.
Additionally, tests with CTest can be run with
`./cmake/run_cmake_ctest.sh Debug`
