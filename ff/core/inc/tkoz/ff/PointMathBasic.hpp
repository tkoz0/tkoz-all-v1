#pragma once

#include <tkoz/ff/CMath.hpp>
#include <tkoz/ff/PointData.hpp>
#include <tkoz/ff/PointNormTags.hpp>
#include <tkoz/ff/Types.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>

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
/// TODO replace all use of std:: math functions with custom wrappers that take
/// Number<float> or Number<double>.
struct PointMathBasic {
  PointMathBasic() = delete;

  //
  // Basic vector operations on components
  //

  /// Adds `left` and `right` (`left + right`)
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  add(PointData<N, T> const &left, PointData<N, T> const &right) noexcept
      -> PointData<N, T> {
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = left[i] + right[i];
    }
    return result;
  }

  /// Adds `right` to `left` in place (`left += right`).
  template <std::size_t N, cNumberType T>
  static constexpr inline void addEq(PointData<N, T> &left,
                                     PointData<N, T> const &right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] += right[i];
    }
  }

  /// Subtracts `left` and `right` (`left - right`)
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  sub(PointData<N, T> const &left, PointData<N, T> const &right) noexcept
      -> PointData<N, T> {
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = left[i] - right[i];
    }
    return result;
  }

  /// Subtracts `right` from `left` in place (`left -= right`).
  template <std::size_t N, cNumberType T>
  static constexpr inline void subEq(PointData<N, T> &left,
                                     PointData<N, T> const &right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] -= right[i];
    }
  }

  /// Multiplies `left` by `right` (`left * right`)
  template <std::size_t N, cNumberType T, typename U>
    requires std::is_convertible_v<U, T>
  [[nodiscard]] static constexpr inline auto mul(PointData<N, T> const &left,
                                                 U right) noexcept
      -> PointData<N, T> {
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = left[i] * right;
    }
    return result;
  }

  /// Multiplies `left` by `right` in place (`left *= right`).
  template <std::size_t N, cNumberType T, typename U>
    requires std::is_convertible_v<U, T>
  static constexpr inline void mulEq(PointData<N, T> &left, U right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] *= right;
    }
  }

  /// Divides `left` by `right` (`left / right`)
  template <std::size_t N, cNumberType T, typename U>
    requires std::is_convertible_v<U, T>
  [[nodiscard]] static constexpr inline auto div(PointData<N, T> const &left,
                                                 U right) noexcept
      -> PointData<N, T> {
    if constexpr (N >= 2) {
      return mul(left, cNumInteger<T, 1> / right);
    } else {
      PointData<N, T> result;
      for (std::size_t i = 0; i < N; ++i) {
        result[i] = left[i] / right;
      }
      return result;
    }
  }

  /// Divides `left` by `right` in place (`left /= right`).
  template <std::size_t N, cNumberType T, typename U>
    requires std::is_convertible_v<U, T>
  static constexpr inline void divEq(PointData<N, T> &left, U right) noexcept {
    if constexpr (N >= 2) {
      // Precompute 1/right and multiply instead
      // TODO is N >= 2 the right threshold?
      // The answer may differ by an ulp, but in practice the speed gain from
      // this method is worth it for being a single ulp off sometimes.
      mulEq(left, cNumInteger<T, 1> / right);
    } else {
      for (std::size_t i = 0; i < N; ++i) {
        left[i] /= right;
      }
    }
  }

  /// Multiplies vector components: `(left0*right0,left1*right1,...)`
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  componentMul(PointData<N, T> const &left,
               PointData<N, T> const &right) noexcept -> PointData<N, T> {
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = left[i] * right[i];
    }
    return result;
  }

  /// Multiplies vector components in place: `(left0*right0,left1*right1,...)`.
  template <std::size_t N, cNumberType T>
  static constexpr inline void
  componentMulEq(PointData<N, T> &left, PointData<N, T> const &right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] *= right[i];
    }
  }

  /// Divides vector components: `(left0/right0,left1/right1,...)`
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  componentDiv(PointData<N, T> const &left,
               PointData<N, T> const &right) noexcept -> PointData<N, T> {
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = left[i] / right[i];
    }
    return result;
  }

  /// Divides vector components in place: `(left0/right0,left1/right1,...)`.
  template <std::size_t N, cNumberType T>
  static constexpr inline void
  componentDivEq(PointData<N, T> &left, PointData<N, T> const &right) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      left[i] /= right[i];
    }
  }

  /// Computes a fused multiply add of `left*scale + right`
  template <std::size_t N, cNumberType T, typename U>
    requires std::is_convertible_v<U, T>
  [[nodiscard]] static constexpr inline auto
  fma(PointData<N, T> const &left, U scale,
      PointData<N, T> const &right) noexcept -> PointData<N, T> {
    PointData<N, T> result;
    using F = T::FpType;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = std::fma(F{left[i]}, F{scale}, F{right[i]});
    }
    return result;
  }

  //
  // Mathematical vector operations
  //

  /// \return Dot product: `(left0*right0 + left1*right1 + ...)`.
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

  /// \return Angle between vectors (in radians).
  /// \tparam cAssumeUnitT If true, inputs are assumed to be unit vectors.
  /// \note This computes L2 norms as well when not assuming unit vectors.
  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  angleBetween(PointData<N, T> left, PointData<N, T> right) noexcept -> T {
    using F = T::FpType;
    if constexpr (N == 1) {
      // Handle 1d separately by using signs
      return std::signbit(F{left.template at<0>()}) ==
                     std::signbit(F{right.template at<0>()})
                 ? F{0.0}
                 : cNumPi<T>;
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
      return cNumInteger<T, 2> *
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

  /// \return The 2D "cross" product (`left0*right1 - left1*right0`).
  template <cNumberType T>
  [[nodiscard]] static constexpr inline auto
  cross2d(PointData<2, T> const &left, PointData<2, T> const &right) noexcept
      -> T {
    // Use std::fma to calculate with 2 roundings instead of 3
    // For overflow concerns, this is doable by scale down then back up
    using F = T::FpType;
    return std::fma(F{left[0]}, F{right[1]}, -F{left[1] * right[0]});
  }

  /// \return The 3D cross product which is perpendicular to both inputs.
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

  /// \return Linear interpolation of 2 vectors (`t*left + (1-t)*right`).
  /// \note Intended for t in [0,1] but will continue linearly outside of that.
  template <std::size_t N, cNumberType T, typename U>
    requires std::is_convertible_v<U, T>
  [[nodiscard]] static constexpr inline auto
  interpolate(PointData<N, T> const &left, PointData<N, T> const &right,
              U t) noexcept -> PointData<N, T> {
    // Note: can compute lerp(a,b,t) either as (1-t)*a + t*b, or a + t*(b-a)
    // The second way requires 1 multiplication and is expected to be faster.
    // The second one also is supposedly better numerically.
    PointData<N, T> result;
    // const T ta = T{1.0} - t;
    for (std::size_t i = 0; i < N; ++i) {
      // result[i] = (ta * left[i]) + (t * right[i]);
      result[i] = left[i] + (t * (right[i] - left[i]));
    }
    return result;
  }

  /// \return The midpoint of 2 vectors (`(left+right)/2`).
  /// \note This is a special case of midpoint with t=0.5.
  template <std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  midpoint(PointData<N, T> const &left, PointData<N, T> const &right) noexcept
      -> PointData<N, T> {
    // Note: the lerp form may be more numerically stable and should compile
    // to just as efficient code.
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = T{static_cast<T::FpType>(0.5)} * (left[i] + right[i]);
      // result[i] = left[i] + (0.5 * (right[i] - left[i]));
    }
    return result;
  }

  //
  // Vector component operations
  //

  // project onto vector in place (optionally assume onto is a unit vector)
  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  projectOnto(PointData<N, T> const &dir, PointData<N, T> const &onto) noexcept
      -> PointData<N, T> {
    T scale = dot(dir, onto);
    if constexpr (!cAssumeUnitT) {
      scale /= dotProduct(onto, onto);
    }
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = scale * onto[i];
    }
    return result;
  }

  // rejection of a vector (orthogonal to projection) (may assume from is unit)
  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  rejectFrom(PointData<N, T> const &dir, PointData<N, T> const &from) noexcept
      -> PointData<N, T> {
    T scale = -dot(dir, from);
    if constexpr (!cAssumeUnitT) {
      scale /= dotProduct(from, from);
    }
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = std::fma(scale, from[i], dir[i]);
    }
    return result;
  }

  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  parallelAndOrthogonalComponents(PointData<N, T> const &dir,
                                  PointData<N, T> const &axis) noexcept
      -> std::pair<PointData<N, T>, PointData<N, T>> {
    PointData<N, T> resultParallel;
    PointData<N, T> resultOrthogonal;
    T scale = dot(dir, axis);
    if constexpr (!cAssumeUnitT) {
      scale /= dotProduct(axis, axis);
    }
    T scaleOrth = -scale;
    for (std::size_t i = 0; i < N; ++i) {
      resultParallel[i] = scale * axis[i];
    }
    for (std::size_t i = 0; i < N; ++i) {
      resultOrthogonal[i] = std::fma(scaleOrth, axis[i], dir[i]);
    }
    return std::make_pair(resultParallel, resultOrthogonal);
  }

  // reflect across normal plane (optionally assume normal is a unit vector)
  // this is like a mirror, keeps same orthogonal component
  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  reflectAcrossPlane(PointData<N, T> const &dir,
                     PointData<N, T> const &normal) noexcept
      -> PointData<N, T> {
    T scale = T{static_cast<T::FpType>(-2.0)} * dot(dir, normal);
    if constexpr (!cAssumeUnitT) {
      scale /= dotProduct(normal, normal);
    }
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = std::fma(scale, normal[i], dir[i]);
    }
    return result;
  }

  // reflect along an axis (optionally assume axis is a unit vector)
  // this is opposite of reflectAcrossPlane, keeps the same parallel component
  template <bool cAssumeUnitT, std::size_t N, cNumberType T>
  [[nodiscard]] static constexpr inline auto
  reflectAlongAxis(PointData<N, T> const &dir,
                   PointData<N, T> const &axis) noexcept -> PointData<N, T> {
    T scale = T{static_cast<T::FpType>(2.0)} * dot(dir, axis);
    if constexpr (!cAssumeUnitT) {
      scale /= dotProduct(axis, axis);
    }
    PointData<N, T> result;
    for (std::size_t i = 0; i < N; ++i) {
      result[i] = std::fma(scale, axis[i], -dir[i]);
    }
    return result;
  }

  //
  // Special extras for 2D
  //

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

  //
  // Vector norms
  //

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
// - other operations besides std::fma which may be worth looking into
//   - rsqrt, hypot, simd

} // namespace tkoz::ff
