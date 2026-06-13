/// Utility functions for calculating error between numbers and checking that
/// they fit within tolerances. Meant to be used for unit tests.

#pragma once

#include <tkoz/ff/Number.hpp>
#include <tkoz/ff/Types.hpp>

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>

namespace tkoz::ff {

// Helpers for floating point error and checks
namespace internal {
template <std::floating_point> struct FpBits;
template <> struct FpBits<float> {
  using Signed = std::int32_t;
  using Unsigned = std::uint32_t;
};
template <> struct FpBits<double> {
  using Signed = std::int64_t;
  using Unsigned = std::uint64_t;
};
} // namespace internal

/// Absolute error between 2 numbers.
template <cNumberType T> auto errAbs(T a, T b) -> T {
  return T{std::abs(a.value() - b.value())};
}

/// Relative error between 2 numbers (using max absolute value).
template <cNumberType T> auto errRel(T a, T b) -> T {
  return T{errAbs(a, b).value() /
           std::max(std::abs(a.value()), std::abs(b.value()))};
}

/// ULP (units in last place) error between 2 numbers.
template <cNumberType T>
auto errUlp(T a, T b) -> internal::FpBits<typename T::FpType>::Unsigned {
  using ReturnT = internal::FpBits<typename T::FpType>::Unsigned;
  using BitsT = internal::FpBits<typename T::FpType>::Signed;
  if (std::isnan(a.value()) || std::isnan(b.value())) {
    return std::numeric_limits<ReturnT>::max();
  }
  if (a == b) {
    return ReturnT{0}; // This handles infinities and +0 == -0
  }
  BitsT bitsA = std::bit_cast<BitsT>(a.value());
  BitsT bitsB = std::bit_cast<BitsT>(b.value());
  // Positive float values as bits correspond to positive signed integer values.
  // Negative float values are ordered backwards, switch negative signed values.
  // This also ensures +0 and -0 map to the same representation (all zero bits).
  if (bitsA < BitsT{0}) {
    bitsA = std::numeric_limits<BitsT>::min() - bitsA;
  }
  if (bitsB < BitsT{0}) {
    bitsB = std::numeric_limits<BitsT>::min() - bitsB;
  }
  return bitsA > bitsB
             ? static_cast<ReturnT>(bitsA) - static_cast<ReturnT>(bitsB)
             : static_cast<ReturnT>(bitsB) - static_cast<ReturnT>(bitsA);
}

/// True if absolute error is at most \a tol
template <cNumberType T> inline auto isCloseAbs(T a, T b, T tol) -> bool {
  return errAbs(a, b) <= tol;
}

/// True if relative error is at most \a tol
template <cNumberType T> inline auto isCloseRel(T a, T b, T tol) -> bool {
  return errRel(a, b) <= tol;
}

/// True if ULP error is at most \a tol
template <cNumberType T>
inline auto
isCloseUlp(T a, T b,
           typename internal::FpBits<typename T::FpType>::Unsigned tol)
    -> bool {
  return errUlp(a, b) <= tol;
}

/// True if absolute and relative errors satisfy tolerances.
template <cNumberType T>
inline auto isClose(T a, T b, T relTol, T absTol) -> bool {
  T const relVal = relTol * std::max(std::abs(a.value()), std::abs(b.value()));
  return errAbs(a, b) <= std::max(relVal, absTol);
}

/// Same as isClose but use same tolerance for absolute and relative.
template <cNumberType T> inline auto isNear(T a, T b, T tol) -> bool {
  return errAbs(a, b) <=
         tol * std::max(typename T::FpType{1.0},
                        std::max(std::abs(a.value()), std::abs(b.value())));
}

/// Compare identity of floating point values. Nans are equal, +0 and -0 are
/// not equal. Otherwise behaves as a standard equality comparison.
template <cNumberType T> inline auto isSame(T a, T b) -> bool {
  return (std::isnan(a.value()) && std::isnan(b.value())) ||
         std::bit_cast<typename internal::FpBits<typename T::FpType>::Unsigned>(
             a.value()) ==
             std::bit_cast<
                 typename internal::FpBits<typename T::FpType>::Unsigned>(
                 b.value());
}

} // namespace tkoz::ff
