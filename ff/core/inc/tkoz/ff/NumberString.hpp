#pragma once

#include <tkoz/ff/Number.hpp>

#include <charconv>
#include <concepts>
#include <format>
#include <stdexcept>
#include <string>
#include <utility>

namespace tkoz::ff {

namespace internal {
template <std::floating_point T> std::size_t fpStrBufSize;
template <> std::size_t fpStrBufSize<float> = 16;
template <> std::size_t fpStrBufSize<double> = 32;
// template <> std::size_t fpStrBufSize<long double> = 64;
} // namespace internal

/// Convert a primitive floating point type to a string. This will use a
/// shortest exact representation which is round trip convertible back.
/// \tparam T Type of the input
/// \param value Floating point value to convert
/// \return \c std::string representation of \c value
template <std::floating_point T> inline auto toString(T value) -> std::string {
  char buf[internal::fpStrBufSize<T>];
  auto const [bufEnd, error] =
      std::to_chars(buf, buf + internal::fpStrBufSize<T>, value);
  // TODO if debugging, assert error == std::errc{}
  std::ignore = error;
  return std::string(buf, bufEnd);
}

/// Convert a wrapped floating point type to a string. This will use a shortest
/// exact representation which is round trip convertible back.
/// \tparam T Type of the input
/// \param value Floating point value to convert
/// \return \c std::string representation of \c value
template <cNumberType T> inline auto toString(T value) -> std::string {
  return toString(static_cast<T::FpType>(value));
}

/// Read a floating point value from a string representation. This will give
/// the closest representable value.
/// \tparam T Result type
/// \tparam cCheckErrorsT Whether to check for conversion errors
/// \return Floating point value for \c input
template <std::floating_point T, bool cCheckErrorsT = true>
inline auto fromString(std::string const &input) -> T {
  T value;
  auto const *ptrBeg = input.data();
  auto const *ptrEnd = input.data() + input.size();
  auto [ptr, error] = std::from_chars(ptrBeg, ptrEnd, value);
  if constexpr (cCheckErrorsT) {
    if (ptr != ptrEnd || error != std::errc{}) {
      throw std::invalid_argument(
          std::format("tkoz::ff::fromString: invalid input: {}", input));
    }
  }
  return value;
}

/// Read a wrapped floating point value from a string representation. This will
/// give the closest representable value.
/// \tparam T Result type
/// \tparam cCheckErrorsT Whether to check for conversion errors
/// \return Floating point value for \c input
template <cNumberType T, bool cCheckErrorsT = true>
inline auto fromString(std::string const &input) -> T {
  return T(fromString<T::FpType, cCheckErrorsT>(input));
}

} // namespace tkoz::ff
