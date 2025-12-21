#include <_test/IntegerMaths.hpp>

#include <cassert>
#include <iostream>
#include <vector>

// The assert() calls are compiled away with -DNDEBUG
// CMake appears to define this for all build types other than Debug

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "no test type argument" << std::endl;
    return 1;
  }
  std::string testType = argv[1];
  if (testType == "small") {
    assert((_test::primeFactorization(2) == std::vector<uint64_t>{2}));
    assert((_test::primeFactorization(3) == std::vector<uint64_t>{3}));
    assert((_test::primeFactorization(4) == std::vector<uint64_t>{2, 2}));
    assert((_test::primeFactorization(10) == std::vector<uint64_t>{2, 5}));
    assert((_test::primeFactorization(12) == std::vector<uint64_t>{2, 2, 3}));
  } else if (testType == "large") {
    assert((_test::primeFactorization(48618463) ==
            std::vector<uint64_t>{1549, 31387}));
    assert((_test::primeFactorization(7846866) ==
            std::vector<uint64_t>{2, 3, 3, 151, 2887}));
  } else if (testType == "other") {
    assert(0);
  } else {
    std::cout << "invalid test type" << std::endl;
    return 1;
  }
  return 0;
}
