# scripts

Note: build areas are in /builds/BuildType-Compiler
- BuildType is one of: Debug, RelWithDebInfo, Release, MinSizeRel, Coverage
- Compiler is one of: GCC, Clang
- All files can be selected for some script parameters with `git ls-files`,
  for example `$(git ls-files '*.cpp')`

Scripts
- `bash_common.sh` contains functions reused in several scripts
- `clang_tidy_fast.sh <files...>` runs clang-tidy "fast" with limited checks (still slow)
- `clang_tidy_full.sh <files...>` runs clang-tidy with all checks enabled
- `cmake_build.sh <type> <comp>` builds the build area of a given type and toolchain
- `cmake_clang_doc.sh <type> <comp>` uses clang-doc to create documentation in a build area
- `cmake_ctest.sh <type> <comp>` runs tests registered with CMake's CTest in a build area
- `cmake_doxygen.sh <type> <comp>` uses doxygen to create documentation in a build area
- `cmake_generate.sh <type> <comp>` creates a build area of a given type and toolchain
- `cov_cleanup.sh <comp>` cleans up generated coverage data files (given the toolchain)
- `cov_generate.sh <comp>` generates coverage html pages from existing coverage data
- `cpp_check.sh` runs cppcheck on all source and header files in the repo
