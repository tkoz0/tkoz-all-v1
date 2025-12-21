#include <_test/Vector2d.hpp>

#include <format>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2 && argc != 3) {
    std::cout << "requires a grid size as 1 or 2 integers" << std::endl;
    return 1;
  }
  _test::Basic2dVector<double> grid;
  if (argc == 2) {
    grid = _test::Basic2dVector<double>(std::atoi(argv[1]), std::atoi(argv[1]));
  } else {
    grid = _test::Basic2dVector<double>(std::atoi(argv[1]), std::atoi(argv[2]));
  }
  for (std::size_t i = 0; i < grid.rows(); ++i) {
    for (std::size_t j = 0; j < grid.cols(); ++j) {
      grid[i, j] = 1.0 / static_cast<double>(i + j + 1);
      if (j > 0) {
        std::cout << " ";
      }
      std::cout << grid[i, j];
    }
    std::cout << std::endl;
  }
  double s = 0.0;
  for (std::size_t i = 0; i < grid.rows(); ++i) {
    for (std::size_t j = 0; j < grid.cols(); ++j) {
      s += grid.at(i, j);
    }
  }
  std::cout << std::format("sum  = {}", s) << std::endl;
  return 0;
}
