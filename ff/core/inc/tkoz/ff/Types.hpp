/// Defines the types for real numbers and histogram counters.

#pragma once

#include <cstdint>

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

/// Real number type for 32 bit mode
using NumberValue32 = float;

/// Histogram counter type for 32 bit mode
using HistogramValue32 = std::uint32_t;

/// Real number type for 64 bit mode
using NumberValue64 = double;

/// Histogram counter type for 64 bit mode
using HistogramValue64 = std::uint64_t;

// TODO should a compile time split be made for 32 bit and 64 bit modes?

namespace detail {

#ifdef TKOZ_FF_32 // 32 bit mode

using NumberValue = float;
using HistogramValue = std::uint32_t;

#else
#ifdef TKOZ_FF_64 // 64 bit mode

using NumberValue = double;
using HistogramValue = std::uint64_t;

#else

// #error "need TKOZ_FF_32 or TKOZ_FF_64 macro"

#endif
#endif

} // namespace detail

} // namespace tkoz::ff
