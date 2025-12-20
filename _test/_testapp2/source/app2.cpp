// test

#include <tkoz/_test/test1.hpp>
#include <tkoz/_test/test2.hpp>

#include <algorithm>
#include <iostream>

int main(int argc, char **argv) {
  std::ignore = argc;
  std::ignore = argv;
  // std::vector<double> a = {1.0, 2.0, 3.0};
  // std::vector<double> b = {4.0, 5.0, 6.0};
  // std::vector<double> c = {7.0, 8.0, 9.0};
  // auto _ = tkoz::_test::concat_vectors<double>(a, b, c);
  // std::ranges::for_each(_, [](double x) { std::cout << x << std::endl; });
  std::vector<double> a = {1.0, 2.0, 3.0};
  std::vector<double> b = {4.0, 5.0, 6.0};
  std::vector<double> c = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  tkoz::_test::mystery2(a, b, c);
  std::ranges::for_each(c, [](double x) { std::cout << x << std::endl; });
}
