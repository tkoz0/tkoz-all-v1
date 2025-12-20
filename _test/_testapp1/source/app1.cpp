// test

#include <tkoz/_test/test1.hpp>
#include <tkoz/_test/test2.hpp>

#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  // int z; // unused
  if (argc <= 1) {
    std::cout << "no arg provided" << std::endl;
    return 1;
  } else {
    int i = std::atoi(argv[1]);
    std::cout << "input number is " << i << std::endl;
    tkoz::_test::mystery(5, 10, 15, i);
    std::cout << "output number is " << i << std::endl;
    return 0;
  }
}
