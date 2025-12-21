#include <_test/IntegerMaths.hpp>

#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "need 1 argument" << std::endl;
    return 1;
  }
  auto factors = _test::primeFactorization(std::atoll(argv[1]));
  for (auto factor : factors) {
    std::cout << factor << std::endl;
  }
  return 0;
}
