#include <_test/IntegerMaths.hpp>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace _test {

/// Finds prime factors of a number, smallest to largest, with multiplicity.
/// \param n number to factor
/// \return prime factors smallest to largest counting multiplicity
std::vector<uint64_t> primeFactorization(uint64_t n) {
  std::vector<uint64_t> result;
  if (n < 2)
    return result;
  while (n % 2 == 0) {
    n /= 2;
    result.push_back(2);
  }
  uint64_t d = 3;
  while (d * d <= n) {
    while (n % d == 0) {
      n /= d;
      result.push_back(d);
    }
    d += 2;
  }
  if (n != 1) {
    result.push_back(n);
  }
  return result;
}

/// Finds all positive divisors from smallest to largest.
/// \param n number to list divisors of
/// \return positive divisors from smallest to largest
std::vector<uint64_t> listDivisors(uint64_t n) {
  std::vector<uint64_t> result;
  std::vector<uint64_t> resultBack;
  uint64_t d = 1;
  while (d * d < n) {
    if (n % d == 0) {
      result.push_back(d);
      resultBack.push_back(n / d);
    }
    ++d;
  }
  if (d * d == n) {
    result.push_back(d);
  }
  std::copy(resultBack.rbegin(), resultBack.rend(), std::back_inserter(result));
  return result;
}

} // namespace _test

#if TEST

#include <tkoz/SRTest.hpp>

TEST_CREATE(smallFactors) {
  TEST_REQUIRE((_test::primeFactorization(5) == std::vector<uint64_t>{5}));
  TEST_REQUIRE(
      (_test::primeFactorization(18) == std::vector<uint64_t>{2, 3, 3}));
  TEST_REQUIRE(
      (_test::primeFactorization(28) == std::vector<uint64_t>{2, 2, 7}));
  TEST_REQUIRE((_test::primeFactorization(1) == std::vector<uint64_t>{}));
}

TEST_CREATE(largeFactors) {
  TEST_REQUIRE(_test::primeFactorization(1000003) ==
               std::vector<uint64_t>{1000003});
  TEST_REQUIRE((_test::primeFactorization(1022117) ==
                std::vector<uint64_t>{1009, 1013}));
  TEST_REQUIRE(
      (_test::primeFactorization(994009) == std::vector<uint64_t>{997, 997}));
}

#endif // TEST
