#pragma once

#include <tkoz/ff/CMath.hpp>
#include <tkoz/ff/PointData.hpp>
#include <tkoz/ff/PointNormTags.hpp>
#include <tkoz/ff/Types.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <type_traits>

namespace tkoz::ff {

/// \brief Basic reasonable implementation for math operations on points.
///
/// These implementations are a starting point for the algorithms to use.
/// Further math policies later may optimize for speed or stability differently.
/// These also define the interface expected of math policies.
///
/// In general, the speed of the math does little to the rendering speed which
/// is dominated by random memory access.
///
struct PointMathBasic {
  PointMathBasic() = delete;

  // left += right.
  template <std::size_t N, cNumberType T>
  static constexpr inline void addEq(PointData<N, T> &left,
                                     PointData<N, T> const &right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] += right[i];
    }
  }

  // left -= right.
  template <std::size_t N, cNumberType T>
  static constexpr inline void subEq(PointData<N, T> &left,
                                     PointData<N, T> const &right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] -= right[i];
    }
  }

  // left *= right.
  template <std::size_t N, cNumberType T>
  static constexpr inline void mulEq(PointData<N, T> &left, T right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] *= right;
    }
  }

  // left /= right.
  template <std::size_t N, cNumberType T>
  static constexpr inline void divEq(PointData<N, T> &left, T right) noexcept {
    if constexpr (N >= 2) {
      // Precompute 1/right and multiply instead
      // TODO is N >= 2 the right threshold?
      // The answer may differ by an ulp, but in practice the speed gain from
      // this method is worth it for being a single ulp off sometimes.
      mulEq(left, cNumInteger<typename T::FpType, 1> / right);
    } else {
      for (std::size_t i = 0; i < N; ++i) {
        left[i] /= right;
      }
    }
  }

  // component multiply
  template <std::size_t N, cNumberType T>
  static constexpr inline void
  componentMulEq(PointData<N, T> &left, PointData<N, T> const &right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] *= right[i];
    }
  }

  // component divide
  template <std::size_t N, cNumberType T>
  static constexpr inline void
  componentDivEq(PointData<N, T> &left, PointData<N, T> const &right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] /= right[i];
    }
  }

  // dot product
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  dotProduct(PointData<N, T> const &left, PointData<N, T> const &right) noexcept
      -> T {
    // This is fine for small N. For large N, we may want Kahan/pairwise
    // summation, which may reduce error from O(N*eps) to O(log(N)*eps)
    using F = T::FpType;
    F result{0.0};
    for (std::size_t i = 0; i < N; ++i) {
      result = std::fma(F{left[i]}, F{right[i]}, result);
    }
    return result;
  }

  // angle between vectors
  // note: this computes L2 norms as well when not assuming unit vectors
  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  angleBetween(PointData<N, T> left, PointData<N, T> right) noexcept -> T {
    using F = T::FpType;
    if constexpr (N == 1) {
      // Handle 1d separately by using signs
      return std::signbit(F{left.template at<0>()}) ==
                     std::signbit(F{right.template at<0>()})
                 ? F{0.0}
                 : cNumPi<F>;
    } else if constexpr (N == 2) {
      // Special case 2d with "cross product"
      return std::atan2(std::abs(F{cross2d(left, right)}),
                        F{dotProduct(left, right)});
    } else if constexpr (N == 3) {
      // Special case 3d with cross product
      return std::atan2(F{pNormIntCt<2>(cross3d(left, right))},
                        F{dotProduct(left, right)});
    } else {
      // In general, the half angle atan2 method is the best balance of speed
      // and stability.
      if constexpr (!cAssumeUnitT) {
        T const leftNorm = pNormIntCt<2>(left);
        T const rightNorm = pNormIntCt<2>(right);
        mulEq(left, rightNorm);
        mulEq(right, leftNorm);
      }
      auto sub = left;
      auto add = left;
      subEq(sub, right);
      addEq(add, right);
      return cNumInteger<F, 2> *
             std::atan2(F{pNormIntCt<2>(sub)}, F{pNormIntCt<2>(add)});
    }

    // Alternative formulas
    // Standard acos(dot(a,b)/(norm(a)*norm(b))) - not stable near 0 or pi
    // In 2d and 3d, atan2(cross(a,b),dot(a,b))
    // asin(cross(a,b)/(norm(a)*norm(b)))
    // 2*atan2(norm(a-b),norm(a+b)) - good stability, form for unit vectors
    // 2*atan2(norm(norm(b)*a-norm(a)*b),norm(norm(b)*a+norm(a)*b))
    // Gram determinant form which may overflow
    // atan2(sqrt(norm(a)^2*norm(b)^2-dot(a,b)^2),dot(a,b))
  }

  // 2d "cross" product (the z component as if done in 3 dimensions)
  template <cNumberType T>
  [[nodiscard]] static constexpr inline auto
  cross2d(PointData<2, T> const &left, PointData<2, T> const &right) noexcept
      -> T {
    // Use std::fma to calculate with 2 roundings instead of 3
    // For overflow concerns, this is doable by scale down then back up
    using F = T::FpType;
    return std::fma(F{left[0]}, F{right[1]}, -F{left[1] * right[0]});
  }

  // 3d cross product
  template <cNumberType T>
  [[nodiscard]] static constexpr inline auto
  cross3d(PointData<3, T> const &left, PointData<3, T> const &right) noexcept
      -> PointData<3, T> {
    // Use std::fma to calculate each component with 2 roundings instead of 3
    // For overflow concerns, this is doable by scale down then back up
    using F = T::FpType;
    return PointData<3, T>(
        std::fma(F{left[1]}, F{right[2]}, -F{left[2] * right[1]}),
        std::fma(F{left[2]}, F{right[0]}, -F{left[0] * right[2]}),
        std::fma(F{left[0]}, F{right[1]}, -F{left[1] * right[0]}));
  }

  // linear interpolation
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  interpolate(PointData<N, T> const &left, PointData<N, T> const &right,
              T t) noexcept -> PointData<N, T> {
    // Note: can compute lerp(a,b,t) either as (1-t)*a + t*b, or a + t*(b-a)
    // The second way requires 1 multiplication and is expected to be faster.
    // The second one also is supposedly better numerically.
    PointData<N, T> result;
    // const T ta = T{1.0} - t;
    for (std::size_t i = 0; i < N; ++i) {
      // result.mData[i] = (ta * left.mData[i]) + (t * right.mData[i]);
      result[i] = left[i] + (t * (right[i] - left[i]));
    }
    return result;
  }

  // midpoint (special case of interpolate)
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  midpoint(PointData<N, T> const &left, PointData<N, T> const &right) noexcept
      -> PointData<N, T> {
    // Note: the lerp form may be more numerically stable and should compile
    // to just as efficient code.
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result.mData[i] = (left.mData[i] + right.mData[i]) / T{2.0};
      // result.mData[i] =
      //     left.mData[i] + (0.5 * (right.mData[i] - left.mData[i]));
    }
    return result;
  }

  // project onto vector in place (optionally assume onto is a unit vector)
  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  static constexpr inline void
  projectOnto(PointData<N, T> &point, PointData<N, T> const &onto) noexcept {
    T scale = dot(point, onto);
    if constexpr (!cAssumeUnitT) {
      scale /= pNormIntPowerSumCt<2>(onto);
    }
    for (std::size_t i = 0; i < N; ++i) {
      point[i] = scale * onto[i];
    }
  }

  // reflect across axis in place (optionally assume axis is a unit vector)
  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  static constexpr inline void
  reflectAcross(PointData<N, T> &point, PointData<N, T> const &axis) noexcept {
    T scale = T{2.0} * dot(point, axis);
    if constexpr (!cAssumeUnitT) {
      scale /= pNormIntPowerSum<2>(axis);
    }
    for (std::size_t i = 0; i < N; ++i) {
      point[i] = (scale * axis[i]) - point[i];
    }
  }

  // ccw rotation in 2d plane (modifies x and y in place)
  template <cNumberType T>
  static constexpr inline void rotate2d(T &x, T &y, T angle) noexcept {
    const auto [sinA, cosA] = cmathSinCos(angle);
    T const oldX = x;
    T const oldY = y;
    x = (oldX * cosA) - (oldY * sinA);
    y = (oldX * sinA) + (oldY * cosA);
  }

  // efficient radius, sine, and cosine in 2d
  template <cNumberType T>
  static constexpr inline void sinCosRad2d(T const &x, T const &y, T &outSin,
                                           T &outCos, T &outRad) noexcept {
    outRad = pNormIntCt<2>(PointData<2, T>(x, y));
    outCos = x / outRad;
    outSin = y / outRad;
  }

  // nicer front end vector norm interface that does not use 5 function names
  // pass an instance of PNormCt or PNormRt, calls the appropriate function
  template <std::size_t N, cNumberType T, cIsNormType NormT>
  [[nodiscard]] static constexpr inline auto norm(PointData<N, T> const &point,
                                                  NormT norm) noexcept -> T {
    if constexpr (std::is_same_v<NormT, InfNorm>) {
      return maxNorm(point);
    } else if constexpr (cIsPNormCt<NormT>) {
      // Compile time p
      if constexpr (std::is_same_v<typename NormT::PType, bool>) {
        static_assert(false, "PNormCt cannot have a bool value for p");
      } else if constexpr (std::is_integral_v<typename NormT::PType>) {
        // p is integral
        return pNormIntCt<NormT::cPValue>(point);
      } else if constexpr (std::is_floating_point_v<typename NormT::PType>) {
        // p is floating point
        return pNormFloatCt<NormT::cPValue>(point);
      } else {
        static_assert(false, "Invalid instantiation of PNormCt");
      }
    } else if constexpr (cIsPNormRt<NormT>) {
      // Run time p
      if constexpr (std::is_same_v<typename NormT::PType, bool>) {
        static_assert(false, "PNormRt cannot have a bool value for p");
      } else if constexpr (std::is_integral_v<typename NormT::PType>) {
        // p is integral
        return pNormIntRt(point, norm.mPValue);
      } else if constexpr (std::is_floating_point_v<typename NormT::PType>) {
        // p is floating point
        return pNormFloatRt(point, norm.mPValue);
      } else {
        static_assert(false, "Invalid instantiation of PNormRt");
      }
    } else {
      static_assert(false, "Not a valid norm tag");
    }
  }

  // similar to above but for the power sums of p norms
  // power sum is not applicable to the infinity/max norm
  template <std::size_t N, cNumberType T, cIsNormType NormT>
    requires(!std::is_same_v<NormT, InfNorm>)
  [[nodiscard]] static constexpr inline auto
  normPowerSum(PointData<N, T> const &point, NormT norm) noexcept -> T {
    if constexpr (std::is_same_v<NormT, InfNorm>) {
      static_assert(false, "InfNorm does not have a power sum");
    } else if constexpr (cIsPNormCt<NormT>) {
      // Compile time p
      if constexpr (std::is_same_v<typename NormT::PType, bool>) {
        static_assert(false, "PNormCt cannot have a bool value for p");
      } else if constexpr (std::is_integral_v<typename NormT::PType>) {
        // p is integral
        return pNormIntPowerSumCt<NormT::cPValue>(point);
      } else if constexpr (std::is_floating_point_v<typename NormT::PType>) {
        // p is floating point
        return pNormFloatPowerSumCt<NormT::cPValue>(point);
      } else {
        static_assert(false, "Invalid instantiation of PNormCt");
      }
    } else if constexpr (cIsPNormRt<NormT>) {
      // Run time p
      if constexpr (std::is_same_v<typename NormT::PType, bool>) {
        static_assert(false, "PNormRt cannot have a bool value for p");
      } else if constexpr (std::is_integral_v<typename NormT::PType>) {
        // p is integral
        return pNormIntPowerSumRt(point, norm.mPValue);
      } else if constexpr (std::is_floating_point_v<typename NormT::PType>) {
        // p is floating point
        return pNormFloatPowerSumRt(point, norm.mPValue);
      } else {
        static_assert(false, "Invalid instantiation of PNormRt");
      }
    } else {
      static_assert(false, "Not a valid norm tag");
    }
  }

private:
  // Functions for specific p norm use. The norm() and normPowerSum() functions
  // dispatch to one of these.
  // Note: Ct = compile time and Rt = runtime
  // We have runtime p norms for flexibility and compile time for optimization.
  // Additionally they are split up into int and float.
  // Then there are also the power sums before the root.
  // So there are 8 p norm functions but we can select for optimization.

  // p norm power sum (integer, compile time)
  // TODO maybe a few more special cases because std::pow is expensive
  template <auto P, std::size_t N, cNumberType T>
    requires(std::is_integral_v<decltype(P)> &&
             !std::is_same_v<decltype(P), bool> && P >= decltype(P){1})
  [[nodiscard]] static constexpr inline auto
      pNormIntPowerSumCt(PointData<N, T> const &point) noexcept -> T {
    using F = T::FpType;
    static constexpr F cPT{P};
    F result{0.0};
    if constexpr (P == 1) { // L1 (taxicab)
      for (std::size_t i = 0; i < N; ++i) {
        result += std::abs(F{point[i]});
      }
    } else if constexpr (P == 2) { // L2 (euclidean)
      for (std::size_t i = 0; i < N; ++i) {
        result = std::fma(F{point[i]}, F{point[i]}, result);
      }
    } else if constexpr (P % 2 == 0) { // Even exponent
      for (std::size_t i = 0; i < N; ++i) {
        result += std::pow(F{point[i]}, cPT);
      }
    } else { // Odd exponent
      static_assert(P % 2 == 1);
      for (std::size_t i = 0; i < N; ++i) {
        result += std::pow(std::abs(F{point[i]}), cPT);
      }
    }
    return result;
  }

  // p norm power sum (integer, run time)
  // p < 1 is not a norm, not checked
  // TODO does having special cases make sense? probably not
  // this one is meant to be more general for larger p
  template <typename P, std::size_t N, cNumberType T>
    requires(std::is_integral_v<P> && !std::is_same_v<P, bool>)
  [[nodiscard]] static constexpr inline auto
  pNormIntPowerSumRt(PointData<N, T> const &point, P power) noexcept -> T {
    using F = T::FpType;
    F const powerT{power};
    F result{0.0};
    for (std::size_t i = 0; i < N; ++i) {
      result += std::pow(std::abs(F{point[i]}), powerT);
    }
    return result;
  }

  // p norm (integer, compile time)
  // TODO maybe more special cases make sense since std::pow is slow
  template <auto P, std::size_t N, cNumberType T>
    requires(std::is_integral_v<decltype(P)> &&
             !std::is_same_v<decltype(P), bool> && P >= 1)
  [[nodiscard]] static constexpr inline auto
  pNormIntCt(PointData<N, T> const &point) noexcept -> T {
    using F = T::FpType;
    static constexpr F cInvP = F{1.0} / F{P};
    if constexpr (P == 1) {
      return pNormIntPowerSumCt<1>(point);
    } else if constexpr (P == 2) {
      return std::sqrt(F{pNormIntPowerSumCt<2>(point)});
    } else if constexpr (P == 3) {
      return std::cbrt(F{pNormIntPowerSumCt<3>(point)});
    } else {
      return std::pow(F{pNormIntPowerSumCt<P>(point), cInvP});
    }
  }

  // p norm (integer, run time)
  // p < 1 is not a norm, not checked
  // TODO does having special cases make sense? probably not
  // this one is meant to be more general for larger p
  template <typename P, std::size_t N, cNumberType T>
    requires(std::is_integral_v<P> && !std::is_same_v<P, bool>)
  [[nodiscard]] static constexpr inline auto
  pNormIntRt(PointData<N, T> const &point, P power) noexcept -> T {
    using F = T::FpType;
    F const cInvPower = F{1.0} / F{power};
    return std::pow(F{pNormIntPowerSumRt(point, power)}, cInvPower);
  }

  // max/infinity norm
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  maxNorm(PointData<N, T> const &point) noexcept -> T {
    using F = T::FpType;
    F result{0.0};
    for (std::size_t i = 0; i < N; ++i) {
      result = std::max(result, std::abs(F{point[i]}));
    }
    return result;
  }

  // p norm power sum (float, compile time)
  // no special cases, use integer variants if you want an integer norm
  template <auto P, std::size_t N, cNumberType T>
    requires(std::is_floating_point_v<decltype(P)> && P >= decltype(P){1.0})
  [[nodiscard]] static constexpr inline auto
      pNormFloatPowerSumCt(PointData<N, T> const &point) noexcept -> T {
    using F = T::FpType;
    static constexpr F cPT{P};
    F result{0.0};
    for (std::size_t i = 0; i < N; ++i) {
      result += std::pow(std::abs(F{point[i]}), cPT);
    }
    return result;
  }

  // p norm power sum (float, run time)
  // p < 1 is not a norm, not checked
  // no special cases, use integer variants if you want an integer norm
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  pNormFloatPowerSumRt(PointData<N, T> const &point, T power) noexcept -> T {
    using F = T::FpType;
    F result{0.0};
    for (std::size_t i = 0; i < N; ++i) {
      result += std::pow(std::abs(F{point[i]}), power);
    }
    return result;
  }

  // p norm (float, compile time)
  // p < 1 is not a norm
  // no special cases, use integer variants if you want an integer norm
  template <auto P, std::size_t N, cNumberType T>
    requires(std::is_floating_point_v<decltype(P)> && P >= decltype(P){1.0})
  [[nodiscard]] static constexpr inline auto
      pNormFloatCt(PointData<N, T> const &point) noexcept -> T {
    using F = T::FpType;
    static constexpr F cInvP = F{1.0} / F{P};
    return std::pow(F{pNormFloatPowerSumCt<P>(point)}, cInvP);
  }

  // p norm (float, run time)
  // p < 1 is not a norm, not checked
  // no special cases, use integer variants if you want an integer norm
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  pNormFloatRt(PointData<N, T> const &point, T power) noexcept -> T {
    using F = T::FpType;
    F const invPower = F{1.0} / F{power};
    return std::pow(F{pNormFloatPowerSumRt(point, power)}, invPower);
  }
};

// Notes
// - distance between A,B is just magnitude(A-B) and we cannot do better
//   numerically than using the difference we can determine, so if two
//   corresponding components are very close, we already lost information
// - similarly, normalizing a vector is no better than A/magnitude(A),
//   the only room for better stability is really the magnitude function

} // namespace tkoz::ff
