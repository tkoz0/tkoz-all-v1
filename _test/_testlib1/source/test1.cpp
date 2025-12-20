#include <tkoz/_test/test1.hpp>

#include <vector>

namespace tkoz::_test {

void mystery(const int &a, const int &b, const int &c, int &d) {
  // int i = -1; // unused
  // int j = -2; // unused
  d -= a;
  d += b;
  d -= c;
  d += a * b * c;
}

void mystery2(const std::vector<double> &a, const std::vector<double> &b,
              std::vector<double> &c) {
  if (a.size() != b.size()) {
    throw 0;
  }
  c.resize(2 * a.size());
  auto c1 = c.begin();
  auto c2 = c1 + a.size();
  for (std::size_t i = 0; i < a.size(); ++i) {
    *(c1++) = a[i] + b[i];
    *(c2++) = a[i] * b[i];
  }
}

} // namespace tkoz::_test
