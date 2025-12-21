#include <_test/IntegerMaths.hpp>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace _test {

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
