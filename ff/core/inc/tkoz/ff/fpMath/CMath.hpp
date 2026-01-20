#pragma once

#include <tkoz/ff/Types.hpp>
#include <tkoz/ff/fpMath/Helpers.hpp>
#include <tkoz/ff/fpMath/Numbers.hpp>

#include <cmath>
#include <type_traits>
#include <utility>

namespace tkoz::ff::fpMath {

/// Type generic simultaneous sine and cosine, calling the libm function from
/// \a <cmath>. This may only be available on GCC and Clang and is just as
/// accurate as std::sin and std::cos, and in practice, the most stable choice.
/// \param x The input angle
/// \return A \a std::pair containing \a [sin(x),cos(x)]
template <typename T>
[[nodiscard]] inline constexpr std::pair<std::decay_t<T>, std::decay_t<T>>
cmathSinCos(T &&x) noexcept {
  using U = std::decay_t<T>;
#ifdef __GLIBC__
  U sinX;
  U cosX;
  if constexpr (std::is_same_v<U, float>) {
    ::sincosf(x, &sinX, &cosX);
  } else if constexpr (std::is_same_v<U, double>) {
    ::sincos(x, &sinX, &cosX);
  } else if constexpr (std::is_same_v<U, long double>) {
    ::sincosl(x, &sinX, &cosX);
  } else {
    static_assert(false);
  }
  return std::make_pair(sinX, cosX);
#else // Fall back to standard sin/cos
  return std::make_pair(std::sin(x), std::cos(x));
#endif
}

} // namespace tkoz::ff::fpMath
