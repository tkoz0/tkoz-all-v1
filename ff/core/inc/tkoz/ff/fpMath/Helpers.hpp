#pragma once

#include <cstdint>

namespace tkoz::ff::fpMath {

/// Compile time factorial. Primarily intended for compile time Taylor series.
/// \tparam N An integer from 0 to 20
template <int N>
  requires(N >= 0 && N <= 20)
inline constexpr std::uint64_t cFactorial = N * cFactorial<N - 1>;

/// Specialization for the base case N=0.
template <> inline constexpr std::uint64_t cFactorial<0> = 1;

} // namespace tkoz::ff::fpMath
