#pragma once

#include <array>
#include <concepts>
#include <span>
#include <type_traits>
#include <utility>

namespace tkoz::ff {

/// \brief The storage and access components of a \a Point.
/// \tparam cDimsT Number of dimensions
/// \tparam NumberT Floating point type
template <std::size_t cDimsT, std::floating_point NumberT>
  requires(cDimsT > 0 && cDimsT < 256)
class PointData {
protected:
  // The point/vector storage
  std::array<NumberT, cDimsT> mData;

public:
  /// Types for STL stuff
  using value_type = NumberT;
  using iterator = decltype(mData)::iterator;
  using const_iterator = decltype(mData)::const_iterator;

  /// Number of dimensions
  static constexpr std::size_t cDimensions = cDimsT;

  // === Constructors ===

  /// Create an uninitialized \a PointData. This is for efficiency.
  /// To create a zero initialized instance, use \a PointData::zero().
  [[nodiscard]] inline constexpr PointData() noexcept = default;

  /// Create a zero initialized \a PointData.
  /// \return The zero point/vector (origin).
  [[nodiscard]] inline static PointData zero() noexcept {
    PointData result;
    for (std::size_t i = 0; i < cDimensions; ++i) {
      result.mData[i] = static_cast<NumberT>(0.0);
    }
    return result;
  }

  /// Create a \a PointData from a sequence of numbers.
  /// \param values Sequence of numbers
  /// \tparam InNUmberTs Types of input numbers, convertible to \a NumberT
  template <typename... InNumberTs>
    requires(sizeof...(InNumberTs) == cDimensions &&
             (std::is_convertible_v<InNumberTs, NumberT> && ...))
  [[nodiscard]] inline constexpr explicit PointData(
      InNumberTs &&...values) noexcept
      : mData{static_cast<NumberT>(values)...} {}

  /// Create a \a PointData from a \a std::array
  /// \param data A \a std::array of type convertible to \a NumberT
  /// \tparam InNumberT The data type of input
  template <typename InNumberT>
    requires(std::is_convertible_v<InNumberT, NumberT>)
  [[nodiscard]] inline constexpr explicit PointData(
      const std::array<InNumberT, cDimensions> &data) noexcept {
    for (std::size_t i = 0; i < cDimensions; ++i) {
      mData[i] = static_cast<NumberT>(data[i]);
    }
  }

  /// Create a \a PointData from a \a std::span.
  /// \param data A \a std::span of a type convertible to \a NumberT
  /// \tparam InNumberT The data type of input
  template <typename InNumberT>
    requires(std::is_convertible_v<InNumberT, NumberT>)
  [[nodiscard]] inline constexpr explicit PointData(
      std::span<InNumberT, cDimsT> data) noexcept {
    for (std::size_t i = 0; i < cDimensions; ++i) {
      mData[i] = static_cast<NumberT>(data[i]);
    }
  }

  /// Create a \a PointData from a C array.
  /// \param data A pointer to an array containing the point/vector components
  /// \tparam InNumberT The data type of input
  template <typename InNumberT>
    requires(std::is_convertible_v<InNumberT, NumberT>)
  [[nodiscard]] inline constexpr explicit PointData(
      const InNumberT *data) noexcept {
    for (std::size_t i = 0; i < cDimensions; ++i) {
      mData[i] = static_cast<NumberT>(data[i]);
    }
  }

  // Note: we do not include a std::initializer_list constructor because those
  // use runtime sizes and PointData has a fixed dimension at compile time.
  // For performance reasons, use the fixed number of arguments constructor
  // with a sequence of float/double instead. The curly brace syntax still
  // calls that constructor if you do: Point<3,float> p{1.0f,2.0f,3.0f};
  // If we provide an initializer list constructor, this syntax will instead
  // use it and it will not have the compile time size checks we want.

  // Default the usual copy and move.

  [[nodiscard]] inline constexpr PointData(const PointData &other) noexcept =
      default;
  [[nodiscard]] inline constexpr PointData(PointData &&other) noexcept =
      default;
  inline constexpr PointData &
  operator=(const PointData &other) noexcept = default;
  inline constexpr PointData &operator=(PointData &&other) noexcept = default;

  // === Data access ===

  /// Access the internal array (const).
  [[nodiscard]] inline constexpr const std::array<NumberT, cDimsT> &
  data() const noexcept {
    return mData;
  }

  /// Access the internal array (non const).
  [[nodiscard]] inline constexpr std::array<NumberT, cDimsT> &data() noexcept {
    return mData;
  }

  /// Access the internal array by pointer (const).
  [[nodiscard]] inline constexpr const NumberT *dataPtr() const noexcept {
    return mData.data();
  }

  /// Access the internal array by pointer (non const).
  [[nodiscard]] inline constexpr NumberT *dataPtr() noexcept {
    return mData.data();
  }

  /// Access component (const, no bounds check).
  [[nodiscard]] inline constexpr const NumberT &
  operator[](std::size_t i) const noexcept {
    return mData[i];
  }

  /// Access component (non const, no bounds check).
  [[nodiscard]] inline constexpr NumberT &operator[](std::size_t i) noexcept {
    return mData[i];
  }

  /// Access component (const, with bounds check).
  [[nodiscard]] inline constexpr const NumberT &at(std::size_t i) const {
    return mData.at(i);
  }

  /// Access component (non const, with bounds check).
  [[nodiscard]] inline constexpr NumberT &at(std::size_t i) {
    return mData.at(i);
  }

  /// Access component with compile time index (const).
  template <std::size_t cIndexT>
    requires(cIndexT < cDimsT)
  [[nodiscard]] inline constexpr const NumberT &at() const noexcept {
    return mData[cIndexT];
  }

  /// Access component with compile time index (non const).
  template <std::size_t cIndexT>
    requires(cIndexT < cDimsT)
  [[nodiscard]] inline constexpr NumberT &at() noexcept {
    return mData[cIndexT];
  }

  // === Iterators ===

  [[nodiscard]] inline constexpr auto begin() noexcept { return mData.begin(); }

  [[nodiscard]] inline constexpr auto begin() const noexcept {
    return mData.begin();
  }

  [[nodiscard]] inline constexpr auto cbegin() const noexcept {
    return mData.cbegin();
  }

  [[nodiscard]] inline constexpr auto rbegin() noexcept {
    return mData.rbegin();
  }

  [[nodiscard]] inline constexpr auto rbegin() const noexcept {
    return mData.rbegin();
  }

  [[nodiscard]] inline constexpr auto crbegin() const noexcept {
    return mData.crbegin();
  }

  [[nodiscard]] inline constexpr auto end() noexcept { return mData.end(); }

  [[nodiscard]] inline constexpr auto end() const noexcept {
    return mData.end();
  }

  [[nodiscard]] inline constexpr auto cend() const noexcept {
    return mData.cend();
  }

  [[nodiscard]] inline constexpr auto rend() noexcept { return mData.rend(); }

  [[nodiscard]] inline constexpr auto rend() const noexcept {
    return mData.rend();
  }

  [[nodiscard]] inline constexpr auto crend() const noexcept {
    return mData.crend();
  }

private:
  // Helper function to access a sequence of indexes and convert to another
  // another Point type, possibly of different type and dimension.
  template <std::floating_point ToNumberT, std::size_t... cIndexesT>
    requires((cIndexesT < cDimensions) && ...)
  [[nodiscard]] inline constexpr PointData<sizeof...(cIndexesT), ToNumberT>
  makePointFromIndexes(std::index_sequence<cIndexesT...>) const noexcept {
    return PointData<sizeof...(cIndexesT), ToNumberT>(
        static_cast<ToNumberT>(mData[cIndexesT])...);
  }
};

} // namespace tkoz::ff
