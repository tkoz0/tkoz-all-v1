#pragma once

#include <type_traits>

namespace tkoz::ff {

/// Tag structure for P norms specified at compile time. Acceptable types are
/// integral and floating point except for bool. The value must be >= 1.
template <auto P>
  requires(((std::is_integral_v<decltype(P)> &&
             !std::is_same_v<decltype(P), bool>) ||
            std::is_floating_point_v<decltype(P)>) &&
           (P >= decltype(P){1}))
struct PNormCt {
  static constexpr auto cPValue = P;
  using PType = decltype(cPValue);
};

/// Wrapper for a runtime P norm. Acceptable types are integral and floating
/// point except for bool. Note that p < 1 is not a norm, but there may be
/// weird calculations we want so we leave it to client code to check this.
template <typename T>
  requires((std::is_integral_v<T> && !std::is_same_v<T, bool>) ||
           std::is_floating_point_v<T>)
struct PNormRt {
  T const mPValue;
  using PType = T;
  PNormRt() = delete;
  [[nodiscard]] inline explicit PNormRt(T pValue) noexcept : mPValue(pValue) {}
};

// Deduction guide for PNormRt
template <typename T> PNormRt(T) -> PNormRt<T>;

/// Tag for infinity/max norm.
struct InfNorm {};

// Implementation details for useful concepts
namespace detail {

template <typename> struct IsPNormCtImpl : std::false_type {};
template <auto V> struct IsPNormCtImpl<PNormCt<V>> : std::true_type {};

template <typename> struct IsPNormRtImpl : std::false_type {};
template <typename T> struct IsPNormRtImpl<PNormRt<T>> : std::true_type {};

} // namespace detail

/// Is T a tag for compile time P norm?
template <typename T>
concept cIsPNormCt = detail::IsPNormCtImpl<T>::value;

/// Is T an instance of a runtime P norm parameter?
template <typename T>
concept cIsPNormRt = detail::IsPNormRtImpl<T>::value;

/// Is T a valid norm tag/wrapper type?
template <typename T>
concept cIsNormType =
    cIsPNormCt<T> || cIsPNormRt<T> || std::is_same_v<T, InfNorm>;

/// Tag for L1 norm. Defined as sum of |x_i|.
using L1Norm = PNormCt<1>;

/// Tag for L2 norm. Defined as sqrt(sum of |x_i|^2).
using L2Norm = PNormCt<2>;

/// Tag for LInf norm. Defined as max of |x_i|.
using LInfNorm = InfNorm;

/// Alternative name for L1Norm.
using TaxicabNorm = L1Norm;

/// Alternative name for L2Norm.
using EuclideanNorm = L2Norm;

/// Alternative name for LInfNorm.
using MaxNorm = LInfNorm;

} // namespace tkoz::ff
