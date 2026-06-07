#pragma once

#include <tkoz/ff/Constants.hpp>
#include <tkoz/ff/Factorial.hpp>
#include <tkoz/ff/Types.hpp>

#include <cmath>
#include <concepts>
#include <type_traits>
#include <utility>

namespace tkoz::ff {

/// Type generic simultaneous sine and cosine, calling the libm function from
/// \c <cmath>. This may only be available on GCC and Clang and is just as
/// accurate as std::sin and std::cos, and in practice, the most stable choice.
/// \param x The input angle
/// \return A \c std::pair containing \c [sin(x),cos(x)]
template <std::floating_point T>
[[nodiscard]] inline constexpr auto cmathSinCos(T x) noexcept
    -> std::pair<T, T> {
#ifdef __GLIBC__
  T sinX;
  T cosX;
  if constexpr (std::is_same_v<T, float>) {
    ::sincosf(x, &sinX, &cosX);
  } else if constexpr (std::is_same_v<T, double>) {
    ::sincos(x, &sinX, &cosX);
  } else if constexpr (std::is_same_v<T, long double>) {
    ::sincosl(x, &sinX, &cosX);
  } else {
    static_assert(false);
  }
  return std::make_pair(sinX, cosX);
#else // Fall back to standard sin/cos
  return std::make_pair(std::sin(x), std::cos(x));
#endif
}

} // namespace tkoz::ff
