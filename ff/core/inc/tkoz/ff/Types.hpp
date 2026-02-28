/// Defines the types for real numbers and histogram counters.

#pragma once

#include <type_traits>

namespace tkoz::ff {

/// There are practical reasons to choose either 32 bit or 64 bit values for
/// the rendering buffer.
///
/// Floating point values (for point iteration)
/// - 64 bit gives extra precision (not really necessary for flame fractals)
/// - 32 bit is only slightly faster on CPU
/// - 32 bit allows twice as much of the buffer in CPU caches (biggest benefit)
///
/// Integer (for histogram values)
/// - 64 bit gives plenty of room for counting
/// - 32 bit is still good but may run out on longer renders
///
/// It is possible that 16 bit floats may be acceptable for smaller images
/// (possibly as something like a render preview), but it is not supported.
/// CPUs cannot natively use 16 bit floats and 16 bit counters run into
/// practical limits much faster.
///
/// In practice, it is probably better to run several smaller renders with
/// 32 bit values, then combine them to 64 bit values later. This allows us
/// to render faster since twice as much of the buffer can fit in CPU caches
/// and we do not really lose anything important as long as the render is
/// short enough to limit the counters to 32 bit (still very long). The more
/// important concern is reaching the 32 bit float precision limits sooner
/// when accumulating values for color mode. We still can achieve a fairly
/// long time per render with this though because the histogram buckets that
/// are first affected are the very dense ones that can tolerate more error
/// without affecting the final image appearance.
///
/// TODO Is there a reasonable way to allow buffers to use different size
/// numbers for integer and floating point values?
///
/// TODO should this library be compiled separately for 32 and 64 bit?

/// Allowed primitive floating point types, only float and double currently.
template <typename T>
concept cPrimitiveFpType =
    std::is_same_v<T, float> || std::is_same_v<T, double>;

template <cPrimitiveFpType NumT> class Number;

/// 32 bit number type (float wrapper)
using NumberFp32 = Number<float>;

/// 64 bit number type (double wrapper)
using NumberFp64 = Number<double>;

/// Number types that can be used for the flame fractal rendering.
template <typename T>
concept cNumberType =
    std::is_same_v<T, NumberFp32> || std::is_same_v<T, NumberFp64>;

} // namespace tkoz::ff
