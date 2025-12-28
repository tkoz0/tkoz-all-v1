#pragma once

#include <vector>

namespace _test {

/// \brief Stores data in a fixed size 2d vector.
template <typename T> class Basic2dVector {
private:
  std::vector<std::vector<T>> mData;

public:
  /// Constructs as empty
  Basic2dVector() = default;

  /// Constructs with a fixed size and optional fill value
  /// \param rows number of rows (first index dimension)
  /// \param cols number of cols (second index dimension)
  /// \param fill value to store on initialization
  Basic2dVector(std::size_t rows, std::size_t cols, const T &fill = T())
      : mData(rows, std::vector<T>(cols, fill)) {}

  /// Access by 2d index
  /// \param row first index
  /// \param col second index
  /// \return reference to element at given 2d index
  T &operator[](std::size_t row, std::size_t col) { return mData[row][col]; }

  /// Access by 2d index but return const reference
  const T &operator[](std::size_t row, std::size_t col) const {
    return mData[row][col];
  }

  /// Access by 2d index with bounds checking (non const)
  T &at(std::size_t row, std::size_t col) { return mData.at(row).at(col); }

  /// Access by 2d index with bounds checking (const)
  const T &at(std::size_t row, std::size_t col) const {
    return mData.at(row).at(col);
  }

  /// \return number of rows
  std::size_t rows() const { return mData.size(); }

  /// \return number of cols
  std::size_t cols() const { return mData.empty() ? 0 : mData.front().size(); }
};

} // namespace _test
