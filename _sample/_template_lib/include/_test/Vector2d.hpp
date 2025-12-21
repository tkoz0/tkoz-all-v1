#pragma once

#include <vector>

namespace _test {

template <typename T> class Basic2dVector {
private:
  std::vector<std::vector<T>> mData;

public:
  Basic2dVector() = default;
  Basic2dVector(std::size_t rows, std::size_t cols, const T &fill = T())
      : mData(rows, std::vector<T>(cols, fill)) {}

  T &operator[](std::size_t row, std::size_t col) { return mData[row][col]; }

  const T &operator[](std::size_t row, std::size_t col) const {
    return mData[row][col];
  }

  T &at(std::size_t row, std::size_t col) { return mData.at(row).at(col); }

  const T &at(std::size_t row, std::size_t col) const {
    return mData.at(row).at(col);
  }

  std::size_t rows() const { return mData.size(); }

  std::size_t cols() const { return mData.empty() ? 0 : mData.front().size(); }
};

} // namespace _test
