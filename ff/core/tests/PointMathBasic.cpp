#include <tkoz/ff/PointMathBasic.hpp>

#include <tkoz/SRTest.hpp>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <random>

using tkoz::ff::cNumEps;
using tkoz::ff::cNumPi;
using tkoz::ff::cNumPiMult;
using tkoz::ff::cNumSqrt;
using tkoz::ff::Fp32;
using tkoz::ff::Fp64;
using tkoz::ff::L2Norm;
using tkoz::ff::Number;
using tkoz::ff::PointData;
using tkoz::ff::PointMathBasic;

// Convert a float/double to string which is convertible back to exact value
template <typename T>
  requires std::is_same_v<T, float> || std::is_same_v<T, double>
inline auto fpString(T value) -> std::string {
  char buf[64];
  auto const [bufEnd, error] = std::to_chars(buf, buf + 64, value);
  TEST_REQUIRE_EQ(error, std::errc{});
  return std::string(buf, bufEnd);
}

// Convert a tkoz::ff::Number to string
template <typename T> inline auto fpString(Number<T> value) -> std::string {
  return fpString(value.value());
}

// Create string representation of the point
template <std::size_t N, typename T>
inline auto pointString(PointData<N, T> const &value) -> std::string {
  std::string result;
  result.push_back('(');
  result += fpString(value[0]);
  for (std::size_t j = 1; j < N; ++j) {
    result.push_back(',');
    result += fpString(value[j]);
  }
  result.push_back(')');
  return result;
}

// Create an error message to show the actual point values used
template <std::size_t N, typename T>
[[nodiscard]] auto errorMessageWithNumbers(PointData<N, T> const &actual,
                                           PointData<N, T> const &expected)
    -> std::string {
  static const char *const cTypeName =
      std::is_same_v<T, Fp32>
          ? "float"
          : (std::is_same_v<T, Fp64> ? "double" : "unknown");
  return std::format("Expected {} but computed {} (type is {})",
                     pointString(expected), pointString(actual), cTypeName);
}

// Compare 2 points for equality by absolute error of corresponding components.
template <std::size_t N, typename T, typename U>
  requires std::is_same_v<T, U> || std::is_same_v<U, typename T::FpType>
[[nodiscard]] auto pointsEqCompAbs(PointData<N, T> const &actual,
                                   PointData<N, T> const &expected, U err)
    -> bool {
  using F = T::FpType;
  for (std::size_t i = 0; i < N; ++i) {
    if (std::abs(F{actual[i] - expected[i]}) > err) [[unlikely]] {
      TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
      return false;
    }
  }
  return true;
}

// Compare 2 points for equality by relative error of corresponding components.
template <std::size_t N, typename T, typename U>
  requires std::is_same_v<T, U> || std::is_same_v<U, typename T::FpType>
[[nodiscard]] auto pointsEqCompRel(PointData<N, T> const &actual,
                                   PointData<N, T> const &expected, U err)
    -> bool {
  using F = T::FpType;
  for (std::size_t i = 0; i < N; ++i) {
    if (std::abs(F{actual[i] - expected[i]}) /
            std::max(std::abs(actual[i].value()),
                     std::abs(expected[i].value())) >
        err) [[unlikely]] {
      TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
      return false;
    }
  }
  return true;
}

// Compare 2 points for equality by a near comparison
template <std::size_t N, typename T, typename U>
  requires std::is_same_v<T, U> || std::is_same_v<U, typename T::FpType>
[[nodiscard]] auto pointsEqCompNear(PointData<N, T> const &actual,
                                    PointData<N, T> const &expected, U err)
    -> bool {
  using F = T::FpType;
  for (std::size_t i = 0; i < N; ++i) {
    if (std::abs(F{actual[i] - expected[i]}) >
        err * std::max(F{1.0}, std::max(std::abs(actual[i].value()),
                                        std::abs(expected[i].value()))))
        [[unlikely]] {
      TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
      return false;
    }
  }
  return true;
}

// Compare 2 points for equality with both relative and absolute tolerances
template <std::size_t N, typename T, typename U>
  requires std::is_same_v<T, U> || std::is_same_v<U, typename T::FpType>
[[nodiscard]] auto pointsEqCompClose(PointData<N, T> const &actual,
                                     PointData<N, T> const &expected, U relErr,
                                     U absErr) {
  using F = T::FpType;
  for (std::size_t i = 0; i < N; ++i) {
    if (std::abs(F{actual[i] - expected[i]}) >
        std::max(relErr *
                     std::max(std::abs(F{actual[i]}), std::abs(F{expected[i]})),
                 absErr)) [[unlikely]] {
      TEST_MESSAGE_ALWAYS(errorMessageWithNumbers(actual, expected));
      return false;
    }
  }
  return true;
}

// Compare 2 points for equality by max norm of their difference.
template <std::size_t N, typename T, typename U>
  requires std::is_same_v<T, U> || std::is_same_v<U, typename T::FpType>
[[nodiscard]] auto pointsEqMax(PointData<N, T> const &actual,
                               PointData<N, T> const &expected, U err) -> bool {
  using F = T::FpType;
  F maxComp{0.0};
  for (std::size_t i = 0; i < N; ++i) {
    maxComp = std::max(maxComp, std::abs(F{actual[i] - expected[i]}));
  }
  if (maxComp > err) [[unlikely]] {
    TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
  }
  return maxComp <= err;
}

// Compare 2 points for equality by L1 norm of their difference.
template <std::size_t N, typename T, typename U>
  requires std::is_same_v<T, U> || std::is_same_v<U, typename T::FpType>
[[nodiscard]] auto pointsEqL1(PointData<N, T> const &actual,
                              PointData<N, T> const &expected, U err) -> bool {
  using F = T::FpType;
  F compSum{0.0};
  for (std::size_t i = 0; i < N; ++i) {
    compSum += std::abs(F{actual[i] - expected[i]});
  }
  if (compSum > err) [[unlikely]] {
    TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
  }
  return compSum <= err;
}

// Compare 2 points for equality by magnitude of their difference (euclidean).
template <std::size_t N, typename T, typename U>
  requires std::is_same_v<T, U> || std::is_same_v<U, typename T::FpType>
[[nodiscard]] auto pointsEqL2(PointData<N, T> const &actual,
                              PointData<N, T> const &expected, U err) -> bool {
  using F = T::FpType;
  F dotSum{0.0};
  for (std::size_t i = 0; i < N; ++i) {
    F const diff = F{actual[i] - expected[i]};
    dotSum = std::fma(diff, diff, dotSum);
  }
  if (dotSum > err * err) [[unlikely]] {
    TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
  }
  return dotSum <= err * err;
}

// Compare 2 points for exact equality
template <std::size_t N, typename T>
[[nodiscard]] auto pointsEqExact(PointData<N, T> const &actual,
                                 PointData<N, T> const &expected) -> bool {
  for (std::size_t i = 0; i < N; ++i) {
    if (actual[i] != expected[i]) [[unlikely]] {
      TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
      return false;
    }
  }
  return true;
}

///
/// Manually created tests
///

// Some manually created test cases for addition
TEST_CREATE_FAST(addEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<3, Fp32> a(1.0f, 2.0f, 3.0f);
    PointData<3, Fp32> const b(4.0f, 5.0f, 6.0f);
    PointMathBasic::addEq(a, b);
    PointData<3, Fp32> const c(5.0f, 7.0f, 9.0f);
    TEST_REQUIRE(pointsEqCompNear(a, c, errF));
  }
  {
    PointData<4, Fp64> a(3.1, 3.2, 3.3, -2.5);
    PointData<4, Fp64> const b(1.7, 1.8, 1.9, -3.5);
    PointMathBasic::addEq(a, b);
    PointData<4, Fp64> const c(4.8, 5.0, 5.2, -6.0);
    TEST_REQUIRE(pointsEqCompNear(a, c, errD));
  }
  {
    PointData<1, Fp64> a(3.14);
    PointData<1, Fp64> const b(-3.14);
    PointMathBasic::addEq(a, b);
    TEST_REQUIRE_EQ(a[0], 0.0);
  }
  {
    PointData<6, Fp32> a(1.5f, -6.25f, 14.7f, -1e21f, 5.7e23f, -3.14f);
    PointData<6, Fp32> const b(1e-10f, 1e-10f, -6.8f, -2e21f, -4.6e23f, 2.72f);
    PointMathBasic::addEq(a, b);
    PointData<6, Fp32> const c(1.5f, -6.25f, 7.9f, -3e21f, 1.1e23f, -0.42f);
    TEST_REQUIRE(pointsEqCompNear(a, c, 10.0f * errF));
  }
  {
    PointData<2, Fp64> a(2.0 + 5e-16, 3.0 - 8e-16);
    PointData<2, Fp64> const b(-2.0, -3.0);
    PointMathBasic::addEq(a, b);
    PointData<2, Fp64> const c(0.0, 0.0);
    TEST_REQUIRE(pointsEqCompNear(a, c, errD));
  }
}

// Some manually created test cases for subtraction
TEST_CREATE_FAST(subEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<5, Fp64> a(60.0, 50.0, 40.0, 30.0, 20.0);
    PointData<5, Fp64> const b(70.0, 80.0, 90.0, 100.0, 110.0);
    PointMathBasic::subEq(a, b);
    PointData<5, Fp64> const c(-10.0, -30.0, -50.0, -70.0, -90.0);
    TEST_REQUIRE(pointsEqCompNear(a, c, errD));
  }
  {
    PointData<1, Fp32> a(1.0000001f);
    PointData<1, Fp32> const b(1.0f);
    PointMathBasic::subEq(a, b);
    PointData<1, Fp32> const c(0.0f);
    TEST_REQUIRE(pointsEqCompNear(a, c, errF));
  }
  {
    PointData<3, Fp64> a(1.19, 2.21, 3.23);
    PointData<3, Fp64> const b(-5.16, 2.73, 0.91);
    PointMathBasic::subEq(a, b);
    PointData<3, Fp64> const c(6.35, -0.52, 2.32);
    TEST_REQUIRE(pointsEqCompNear(a, c, errD));
  }
  {
    PointData<2, Fp32> a(1.0000001f, 0.9999999f);
    PointData<2, Fp32> const b(1.0f, 1.0f);
    PointMathBasic::subEq(a, b);
    PointData<2, Fp32> const c(0.0f, 0.0f);
    TEST_REQUIRE(pointsEqCompNear(a, c, errF));
  }
}

TEST_CREATE_FAST(mulEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<3, Fp64> a(1.4, 3.7, 5.9);
    PointMathBasic::mulEq(a, Fp64{3.0});
    PointData<3, Fp64> const b(4.2, 11.1, 17.7);
    TEST_REQUIRE(pointsEqCompNear(a, b, errD));
  }
  {
    PointData<2, Fp32> a(-3.6f, 6.3f);
    PointMathBasic::mulEq(a, Fp32{-0.33333333f});
    PointData<2, Fp32> const b(1.2f, -2.1f);
    TEST_REQUIRE(pointsEqCompNear(a, b, errF));
  }
}

TEST_CREATE_FAST(divEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<3, Fp32> a(-1.5f, 1.2f, 2.0f);
    PointMathBasic::divEq(a, Fp32{-0.2f});
    PointData<3, Fp32> const b(7.5f, -6.0f, -10.0f);
    TEST_REQUIRE(pointsEqCompNear(a, b, errF));
  }
  {
    PointData<2, Fp64> a(-1.65, 3.3);
    PointMathBasic::divEq(a, Fp64{1.1});
    PointData<2, Fp64> const b(-1.5, 3.0);
    TEST_REQUIRE(pointsEqCompNear(a, b, errD));
  }
}

TEST_CREATE_FAST(compMulEqManual1) {
  // static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<7, Fp64> a(-2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0);
    PointData<7, Fp64> const b(-4.0, -3.0, -2.0, -1.0, 0.0, 1.0, 2.0);
    PointMathBasic::componentMulEq(a, b);
    PointData<7, Fp64> const c(8.0, 3.0, 0.0, -1.0, 0.0, 3.0, 8.0);
    TEST_REQUIRE(pointsEqCompNear(a, c, errD));
  }
}

TEST_CREATE_FAST(compDivEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  // static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<5, Fp32> a(-2.0f, -1.0f, 0.0f, 1.0f, 2.0f);
    PointData<5, Fp32> const b(1.0f, 2.0f, 3.0f, 4.0f, 5.0f);
    PointMathBasic::componentDivEq(a, b);
    PointData<5, Fp32> const c(-2.0f, -0.5f, 0.0f, 0.25f, 0.4f);
    TEST_REQUIRE(pointsEqCompNear(a, c, errF));
  }
}

TEST_CREATE_FAST(dotProductManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<4, Fp64> const a(1.4, -3.5, 0.7, -0.3);
    PointData<4, Fp64> const b(-1.6, -2.0, 1.0, 1.7);
    TEST_REQUIRE_NEAR(double{PointMathBasic::dotProduct(a, b)}, 4.95, errD);
  }
  {
    PointData<3, Fp32> const a(6.0f, -3.0f, -2.0f);
    PointData<3, Fp32> const b(5.0f, 14.0f, -6.0f);
    TEST_REQUIRE_NEAR(float{PointMathBasic::dotProduct(a, b)}, 0.0f, errF);
  }
  // TODO Testing for stability of near orthogonal vectors
  // When the dot product is near 0 or nearly equal things are subtracted,
  // the error could get large.
  // (make a new test case for these, maybe dotProductManual2)
}

TEST_CREATE_FAST(angleBetweenManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<6, Fp32> const a(-6.0f, -8.0f, -143.0f, 54.0f, 41.0f, -86.0f);
    PointData<6, Fp32> const b(6.0f, 8.0f, 143.0f, -54.0f, -41.0f, 86.0f);
    TEST_REQUIRE_NEAR(float{PointMathBasic::angleBetween<false>(a, b)},
                      cNumPi<float>, errF);
  }
  {
    PointData<4, Fp64> const a(3.14, -7.958, -61.671, 106.1);
    PointData<4, Fp64> const b(a);
    TEST_REQUIRE_NEAR(double{PointMathBasic::angleBetween<false>(a, b)}, 0.0,
                      errD);
  }
  {
    PointData<2, Fp64> const a(-1.5, 0.5);
    PointData<2, Fp64> const b(0.5, 1.5);
    TEST_REQUIRE_NEAR(double{PointMathBasic::angleBetween<false>(a, b)},
                      (cNumPiMult<double, 1, 2>), errD);
  }
  {
    PointData<3, Fp32> const a(0.0f, 1.0f, 0.0f);
    PointData<3, Fp32> const b(0.0f, 1.0f, cNumSqrt<float, 3>);
    TEST_REQUIRE_NEAR(float{PointMathBasic::angleBetween<false>(a, b)},
                      (cNumPiMult<float, 1, 3>), errF);
  }
  // TODO Testing for stability of near parallel vectors. The standard acos
  // method gets very unstable there. If checking correctness of the atan2
  // method with dot(a,b) = cos(angle)*norm(a)*norm(b), we may need to be
  // careful about stability near pi/2 since dot(a,b) would be near 0 there.
  // (make a new test case for these, maybe angleBetweenManual2)
}

TEST_CREATE_FAST(cross2dManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<2, Fp32> const a(12.0f, 15.0f);
    PointData<2, Fp32> const b(12.0f, -15.0f);
    TEST_REQUIRE_NEAR(float{PointMathBasic::cross2d(a, b)}, -360.0f, errF);
  }
  {
    PointData<2, Fp64> const a(1.3, 0.6);
    PointData<2, Fp64> const b(0.8, 1.3);
    TEST_REQUIRE_NEAR(double{PointMathBasic::cross2d(a, b)}, 1.21, errD);
  }
  {
    PointData<2, Fp32> const a(-6.0f, -5.0f);
    PointData<2, Fp32> const b(-4.1f, 4.1f);
    TEST_REQUIRE_NEAR(float{PointMathBasic::cross2d(a, b)}, -45.1f, errF);
  }
  {
    PointData<2, Fp64> const a(3.3, -4.4);
    PointData<2, Fp64> const b(1.0, 2.0);
    TEST_REQUIRE_NEAR(double{PointMathBasic::cross2d(a, b)}, 11.0, errD);
  }
}

TEST_CREATE_FAST(cross3dManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<3, Fp64> const a(1.7, 0.0, 0.8);
    PointData<3, Fp64> const b(0.8, 0.0, 1.7);
    PointData<3, Fp64> const c(0.0, -2.25, 0.0);
    TEST_REQUIRE(pointsEqCompNear(PointMathBasic::cross3d(a, b), c, errD));
  }
  {
    PointData<3, Fp32> const a(-1.0f, -2.0f, -3.0f);
    PointData<3, Fp32> const b(1.5f, -2.5f, -3.5f);
    PointData<3, Fp32> const c(-0.5f, -8.0f, 5.5f);
    TEST_REQUIRE(pointsEqCompNear(PointMathBasic::cross3d(a, b), c, errF));
  }
  {
    PointData<3, Fp32> const a(-4.0f, 4.3f, 0.0f);
    PointData<3, Fp32> const b(-2.3f, -2.0f, 0.0f);
    PointData<3, Fp32> const c(0.0f, 0.0f, 17.89f);
    TEST_REQUIRE(pointsEqCompNear(PointMathBasic::cross3d(a, b), c, errF));
  }
  {
    PointData<3, Fp64> const a(3.0, -4.0, 5.0);
    PointData<3, Fp64> const b(-11.0, 12.0, -13.0);
    PointData<3, Fp64> const c(-8.0, -16.0, -8.0);
    TEST_REQUIRE(pointsEqCompNear(PointMathBasic::cross3d(a, b), c, errD));
  }
}

// TEST_CREATE_FAST(interpolate) {}

// TEST_CREATE_FAST(midpoint) {}

// TEST_CREATE_FAST(projectOnto) {}

// TEST_CREATE_FAST(reflectAcross) {}

// TEST_CREATE_FAST(rotate2d) {}

// TEST_CREATE_FAST(sinCosRad2d) {}

// TEST_CREATE_FAST(pNormIntPowerSumCt) {}

// TEST_CREATE_FAST(pNormIntPowerSumRt) {}

// TEST_CREATE_FAST(pNormIntCt) {}

// TEST_CREATE_FAST(pNormIntRt) {}

// TEST_CREATE_FAST(maxNorm) {}

// TEST_CREATE_FAST(pNormFloatPowerSumCt) {}

// TEST_CREATE_FAST(pNormFloatPowerSumRt) {}

// TEST_CREATE_FAST(pNormFloatCt) {}

// TEST_CREATE_FAST(pNormFloatRt) {}

///
/// Randomized testing
///

template <std::size_t cDimT, typename NumT, typename FuncT>
  requires std::is_same_v<NumT, Fp32> || std::is_same_v<NumT, Fp64>
void runRandomTests(std::mt19937_64 &rng, std::size_t numTrials,
                    FuncT &&testFunc) {
  // We will the components of the points with first something in [-1,1]
  // Then multiply by a random 2^k (k=0,1,...,7) which tests a wider range
  // Things like overflow/inf/nan still need to be tested separately
  using FpT = NumT::FpType;
  std::uniform_real_distribution<FpT> unifDist(FpT{-1.0}, FpT{1.0});
  std::uniform_int_distribution<unsigned int> powDist(0, 7);
  for (std::size_t i = 0; i < numTrials; ++i) {
    bool result;
    if constexpr (requires(FuncT f) {
                    {
                      f.template operator()<cDimT, NumT>(
                          std::declval<PointData<cDimT, NumT>>(),
                          std::declval<PointData<cDimT, NumT>>())
                    } -> std::same_as<bool>;
                  }) {
      // Binary on 2 points (add, cross, dot, etc)
      PointData<cDimT, NumT> a;
      PointData<cDimT, NumT> b;
      for (std::size_t j = 0; j < cDimT; ++j) {
        a[j] = unifDist(rng) * static_cast<FpT>(1u << powDist(rng));
        b[j] = unifDist(rng) * static_cast<FpT>(1u << powDist(rng));
      }
      result =
          std::forward<FuncT>(testFunc).template operator()<cDimT, NumT>(a, b);
      if (!result) [[unlikely]] {
        TEST_MESSAGE_FAILURE(
            std::format("Error while testing with a={} and b={}",
                        pointString(a), pointString(b)));
      }
      //
    } else if constexpr (requires(FuncT f) {
                           {
                             f.template operator()<cDimT, NumT>(
                                 std::declval<PointData<cDimT, NumT>>(),
                                 std::declval<NumT>())
                           } -> std::same_as<bool>;
                         }) {
      // Binary with a point and number (mul, div, etc)
      PointData<cDimT, NumT> a;
      FpT b = unifDist(rng) * static_cast<FpT>(1u << powDist(rng));
      for (std::size_t j = 0; j < cDimT; ++j) {
        a[j] = unifDist(rng) * static_cast<FpT>(1u << powDist(rng));
      }
      result =
          std::forward<FuncT>(testFunc).template operator()<cDimT, NumT>(a, b);
      if (!result) [[unlikely]] {
        TEST_MESSAGE_FAILURE(
            std::format("Error while testing with a={} and b={}",
                        pointString(a), fpString(b)));
      }
      //
    } else if constexpr (requires(FuncT f) {
                           {
                             f.template operator()<cDimT, NumT>(
                                 std::declval<PointData<cDimT, NumT>>())
                           } -> std::same_as<bool>;
                         }) {
      // Unary on a single point (norm, negative, etc)
      PointData<cDimT, NumT> a;
      for (std::size_t j = 0; j < cDimT; ++j) {
        a[j] = unifDist(rng) * static_cast<FpT>(1u << powDist(rng));
      }
      result =
          std::forward<FuncT>(testFunc).template operator()<cDimT, NumT>(a);
      if (!result) [[unlikely]] {
        TEST_MESSAGE_FAILURE(
            std::format("Error while testing with {}", pointString(a)));
      }
      //
    } else {
      static_assert(false, "Invalid functor type");
    }
    if (!result) [[unlikely]] {
      TEST_FAILURE("Error occurred during random point tests. "
                   "See output for the point values that caused it.");
    }
  }
}

template <std::size_t cCurDimT, std::size_t cMaxDimT, typename FuncT>
void runRandomTestsAllParamsHelper(std::mt19937_64 &rng, std::size_t numTrials,
                                   FuncT &&testFunc) {
  TEST_MESSAGE_FAILURE(
      std::format("Running {} dimensions with float", cCurDimT));
  runRandomTests<cCurDimT, Fp32>(rng, numTrials, std::forward<FuncT>(testFunc));
  TEST_MESSAGE_FAILURE(
      std::format("Running {} dimensions with double", cCurDimT));
  runRandomTests<cCurDimT, Fp64>(rng, numTrials, std::forward<FuncT>(testFunc));
  if constexpr (cCurDimT < cMaxDimT) {
    runRandomTestsAllParamsHelper<cCurDimT + 1, cMaxDimT>(
        rng, numTrials, std::forward<FuncT>(testFunc));
  }
}

template <std::size_t cMinDimT, std::size_t cMaxDimT, typename FuncT>
  requires(cMinDimT <= cMaxDimT)
void runRandomTestsAllParams(std::size_t seed, std::size_t trials,
                             FuncT &&testFunc) {
  std::mt19937_64 rng(seed);
  runRandomTestsAllParamsHelper<cMinDimT, cMaxDimT>(
      rng, trials, std::forward<FuncT>(testFunc));
}

TEST_CREATE_FAST(addEqRandom1) {
  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] + b[i];
        }
        PointMathBasic::addEq(a, b);
        // Should be exactly add parallel components
        return pointsEqExact(a, c);
      });
}

TEST_CREATE_FAST(subEqRandom1) {
  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] - b[i];
        }
        PointMathBasic::subEq(a, b);
        // Should be exactly subtract parallel components
        return pointsEqExact(a, c);
      });
}

TEST_CREATE_FAST(mulEqRandom1) {
  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a, T b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] * b;
        }
        PointMathBasic::mulEq(a, b);
        // Should be exactly multiply by the constant b
        return pointsEqExact(a, c);
      });
}

TEST_CREATE_FAST(divEqRandom1) {
  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a, T b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] / b;
        }
        PointMathBasic::divEq(a, b);
        // We may actually multiply 1/b for performance so approx compare
        return pointsEqCompRel(a, c, cNumEps<typename T::FpType>);
      });
}

TEST_CREATE_FAST(compMulEqRandom1) {
  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] * b[i];
        }
        PointMathBasic::componentMulEq(a, b);
        // Should be exactly multiplied parallel components
        return pointsEqExact(a, c);
      });
}

TEST_CREATE_FAST(compDivEqRandom1) {
  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] / b[i];
        }
        PointMathBasic::componentDivEq(a, b);
        // Should be exactly divided parallel components
        return pointsEqExact(a, c);
      });
}

// Enable this macro to also write error maximums in test output
// This keeps track of the minimum relative/absolute errors needed to pass
#define WRITE_MAX_ERR

#ifdef WRITE_MAX_ERR
#define DECLARE_MAX_ERR_VARS                                                   \
  static float maxRelErrF = 0.0f;                                              \
  static double maxRelErrD = 0.0;                                              \
  static float maxAbsErrF = 0.0f;                                              \
  static double maxAbsErrD = 0.0;
#define UPDATE_MAX_ERR_VARS(var1, var2)                                        \
  F const theRelErr =                                                          \
      std::abs(var1 - var2) / std::max(std::abs(var1), std::abs(var2));        \
  F const theAbsErr = std::abs(var1 - var2);                                   \
  if constexpr (std::is_same_v<F, float>) {                                    \
    if (theRelErr <= theAbsErr) {                                              \
      maxRelErrF = std::max(maxRelErrF, theRelErr);                            \
    }                                                                          \
    if (theAbsErr <= theRelErr) {                                              \
      maxAbsErrF = std::max(maxAbsErrF, theAbsErr);                            \
    }                                                                          \
  } else if constexpr (std::is_same_v<F, double>) {                            \
    if (theRelErr <= theAbsErr) {                                              \
      maxRelErrD = std::max(maxRelErrD, theRelErr);                            \
    }                                                                          \
    if (theAbsErr <= theRelErr) {                                              \
      maxAbsErrD = std::max(maxAbsErrD, theAbsErr);                            \
    }                                                                          \
  } else {                                                                     \
    static_assert(false);                                                      \
  }
#define MESSAGE_MAX_ERR_VARS                                                   \
  TEST_MESSAGE_ALWAYS(                                                         \
      std::format("maxRelErrF = {}", fpStringAndEps(maxRelErrF)));             \
  TEST_MESSAGE_ALWAYS(                                                         \
      std::format("maxRelErrD = {}", fpStringAndEps(maxRelErrD)));             \
  TEST_MESSAGE_ALWAYS(                                                         \
      std::format("maxAbsErrF = {}", fpStringAndEps(maxAbsErrF)));             \
  TEST_MESSAGE_ALWAYS(                                                         \
      std::format("maxAbsErrD = {}", fpStringAndEps(maxAbsErrD)));
#else
#define DECLARE_MAX_ERR_VARS
#define UPDATE_MAX_ERR_VARS(var1, var2)
#define MESSAGE_MAX_ERR_VARS
#endif

// For curiosity, we try to make somewhat tight tolerances for both GCC/Clang
// It's not easy to explain the compiler differences. In practice, prefer a
// stable algorithm when that is important. In exploring the speed/stability
// balance, we may replace with approximations and see how far we can go until
// the results start looking too different.

// Write an error/tolerance as its value and as a multiple of machine epsilon
template <typename T>
[[nodiscard]] auto fpStringAndEps(T value) -> std::string {
  return std::format("{} ({} * eps)", fpString(value),
                     fpString(value / cNumEps<T>));
}

TEST_CREATE_FAST(dotProductRandom1) {
  DECLARE_MAX_ERR_VARS

  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        using F = T::FpType;
        F c{0.0};
        for (std::size_t i = 0; i < N; ++i) {
          c += F{a[i] * b[i]}; // Not using std::fma, should be nearly equal
        }
        F const d = F{PointMathBasic::dotProduct(a, b)};

    // The math behaves differently on each compiler so different tolerances
#ifdef __clang__
        static constexpr F relTol = F{175.0} * cNumEps<F>;
        static constexpr F absTol = F{100.0} * cNumEps<F>;
#elifdef __GNUC__
        static constexpr F relTol = F{120.0} * cNumEps<F>;
        static constexpr F absTol = F{200.0} * cNumEps<F>;
#else
#error "did not write this with a tolerance for other compilers"
#endif

        bool const passRel =
            std::abs(c - d) <= relTol * std::max(std::abs(c), std::abs(d));
        bool const passAbs = std::abs(c - d) <= absTol;
        bool const result = passRel || passAbs;

        UPDATE_MAX_ERR_VARS(c, d)

        if (!result) [[unlikely]] {
          TEST_MESSAGE_ALWAYS(std::format(
              "Simple no FMA for comparing: {}, "
              "PointMathBasic: {}, "
              "relative error: {}, "
              "absolute error: {}",
              fpString(c), fpString(d),
              fpString(std::abs(c - d) / std::max(std::abs(c), std::abs(d))),
              fpString(std::abs(c - d))));
        }
        return result;
      });

  MESSAGE_MAX_ERR_VARS
}

TEST_CREATE_FAST(angleBetweenRandom1) {
  DECLARE_MAX_ERR_VARS

  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        // Test with basic acos method, but handle 1d separately
        using F = T::FpType;
        if constexpr (N == 1) {
          TEST_REQUIRE(a.template at<0>() != F{0.0} &&
                       b.template at<0>() != F{0.0});
          F const c = F{PointMathBasic::template angleBetween<false>(a, b)};
          if (a.template at<0>() * b.template at<0>() < F{0.0}) {
            // Opposite direction
            return std::abs(c - cNumPi<F>) < F{1.0} * cNumEps<F>;
          } else {
            // Same direction
            return F{0.0} <= c && c < F{1.0} * cNumEps<F>;
          }
        } else {
          F aDotB{0.0};
          F aNorm{0.0};
          F bNorm{0.0};
          for (std::size_t i = 0; i < N; ++i) {
            // Not even using std::fma here
            aDotB += F{a[i] * b[i]};
            aNorm += F{a[i] * a[i]};
            bNorm += F{b[i] * b[i]};
          }
          aNorm = std::sqrt(aNorm);
          bNorm = std::sqrt(bNorm);
          F const cosVal = std::clamp(aDotB / (aNorm * bNorm), F{-1.0}, F{1.0});
          F const c = std::acos(cosVal);
          F const d = F{PointMathBasic::template angleBetween<false>(a, b)};

      // Due to instability of the acos method, maybe consider allowing the
      // tolerance go up quite a bit as the angle gets close to 0 or pi
#ifdef __clang__
          // F const edgeDist = std::min(c, cNumPi<F> - c);
          // F const relTol = F{40.0} * cNumEps<F> / std::cbrt(edgeDist);
          F const relTol = F{360.0} * cNumEps<F>;
          F const absTol = F{60.0} * cNumEps<F>;
#elifdef __GNUC__
          F const edgeDist = std::min(c, cNumPi<F> - c);
          F const relTol = F{80.0} * cNumEps<F>;
          // Some huge 55000*eps shows up (double), so we try to use smaller
          // tolerance away from the edges and let it increase near the edges.
          // GCC seems to be less stable than Clang near instability points
          F const absTol = F{220.0} * cNumEps<F> / std::sqrt(edgeDist);
#else
#error "did not write this with a tolerance for other compilers"
#endif
          bool const passRel =
              std::abs(c - d) <= relTol * std::max(std::abs(c), std::abs(d));
          bool const passAbs = std::abs(c - d) <= absTol;
          bool const result = passRel || passAbs;

          UPDATE_MAX_ERR_VARS(c, d)

          if (!result) [[unlikely]] {
            TEST_MESSAGE_ALWAYS(std::format(
                "Simple acos for checking: {}, "
                "PointMathBasic: {}, "
                "relative error: {}, "
                "absolute error: {}, "
                "relative tolerance: {}, "
                "absolute tolerance: {}",
                fpString(c), fpString(d),
                fpString(std::abs(c - d) / std::max(std::abs(c), std::abs(d))),
                fpString(std::abs(c - d)), fpString(relTol), fpString(absTol)));
          }
          return result;
        }
      });

  MESSAGE_MAX_ERR_VARS
}

TEST_CREATE_FAST(angleBetweenRandom2) {
  DECLARE_MAX_ERR_VARS

  runRandomTestsAllParams<1, 10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        // Test with something more numerically stable
        using F = T::FpType;
        F const dot = F{PointMathBasic::dotProduct(a, b)};
        F const aNorm = F{PointMathBasic::norm(a, L2Norm{})};
        F const bNorm = F{PointMathBasic::norm(b, L2Norm{})};
        F const angle = F{PointMathBasic::template angleBetween<false>(a, b)};
        F const v = std::cos(angle) * aNorm * bNorm; // Should be near dot

#ifdef __clang__
        // Error goes up to ~5000*eps relative, probably where dot product is
        // less relatively stable near orthogonality
        F const midDist = std::abs(angle - cNumPiMult<F, 1, 2>);
        F const relTol = F{40.0} * cNumEps<F> / std::sqrt(midDist);
        F const absTol = F{100.0} * cNumEps<F> / std::sqrt(midDist);
#elifdef __GNUC__
        // GCC error seems higher but it's hard to explain compiler differences
        F const midDist = std::abs(angle - cNumPiMult<F, 1, 2>);
        F const relTol = F{60.0} * cNumEps<F> / std::sqrt(midDist);
        F const absTol = F{150.0} * cNumEps<F> / std::sqrt(midDist);
#else
#error "did not write this with a tolerance for other compilers"
#endif

        bool const passRel =
            std::abs(v - dot) <= relTol * std::max(std::abs(v), std::abs(dot));
        bool const passAbs = std::abs(v - dot) <= absTol;
        bool const result = passRel || passAbs;

        UPDATE_MAX_ERR_VARS(v, dot)

        // Allow error tolerance to go up near pi/2 where dot product is near 0
        if (!result) [[unlikely]] {
          TEST_MESSAGE_ALWAYS(std::format(
              "Dot product: {}, "
              "angle: {}, "
              "cos(angle) * norms: {}, "
              "relative error: {}, "
              "absolute error: {}, "
              "relative tolerance: {}, "
              "absolute tolerance: {}",
              fpString(dot), fpString(angle), fpString(v),
              fpString(std::abs(v - dot) /
                       std::max(std::abs(v), std::abs(dot))),
              fpString(std::abs(v - dot)), fpString(relTol), fpString(absTol)));
        }
        return result;
      });

  MESSAGE_MAX_ERR_VARS
}

TEST_CREATE_FAST(cross2dRandom1) {
  DECLARE_MAX_ERR_VARS

  std::mt19937_64 rng(42 /*seed*/);

  auto const func = []<std::size_t N, typename T>(PointData<N, T> a,
                                                  PointData<N, T> b) -> bool {
    static_assert(N == 2);
    using F = T::FpType;
    F const c = F{(a[0] * b[1]) - (a[1] * b[0])};
    F const d = F{PointMathBasic::cross2d(a, b)};

    // The math behaves differently on each compiler so different tolerances
#ifdef __clang__
    static constexpr F relTol = F{10.0} * cNumEps<F>;
    static constexpr F absTol = F{10.0} * cNumEps<F>;
#elifdef __GNUC__
    static constexpr F relTol = F{10.0} * cNumEps<F>;
    static constexpr F absTol = F{10.0} * cNumEps<F>;
#else
#error "did not write this with a tolerance for other compilers"
#endif

    bool const passRel =
        std::abs(c - d) <= relTol * std::max(std::abs(c), std::abs(d));
    bool const passAbs = std::abs(c - d) <= absTol;
    bool const result = passRel || passAbs;

    UPDATE_MAX_ERR_VARS(c, d);

    if (!result) [[unlikely]] {
      TEST_MESSAGE_ALWAYS(std::format(
          "Simple check: {}, "
          "PointMathBasic: {}, "
          "relative error: {}, "
          "absolute error: {}",
          fpString(c), fpString(d),
          fpString(std::abs(c - d) / std::max(std::abs(c), std::abs(d))),
          fpString(std::abs(c - d))));
    }
    return result;
  };

  runRandomTests<2, Fp32>(rng, 500 /*numTrials*/, func);
  runRandomTests<2, Fp64>(rng, 500 /*numTrials*/, func);

  MESSAGE_MAX_ERR_VARS
}

TEST_CREATE_FAST(cross3dRandom1) {
  // DECLARE_MAX_ERR_VARS

  std::mt19937_64 rng(42 /*seed*/);

  auto const func = []<std::size_t N, typename T>(PointData<N, T> a,
                                                  PointData<N, T> b) -> bool {
    static_assert(N == 3);
    using F = T::FpType;
    PointData<N, T> const c((a[1] * b[2]) - (a[2] * b[1]),
                            (a[2] * b[0]) - (a[0] * b[2]),
                            (a[0] * b[1]) - (a[1] * b[0]));
    PointData<N, T> const d = PointMathBasic::cross3d(a, b);

    // The math behaves differently on each compiler so different tolerances
#ifdef __clang__
    static constexpr F relTol = F{10.0} * cNumEps<F>;
    static constexpr F absTol = F{30.0} * cNumEps<F>;
#elifdef __GNUC__
    static constexpr F relTol = F{10.0} * cNumEps<F>;
    static constexpr F absTol = F{120.0} * cNumEps<F>;
#else
#error "did not write this with a tolerance for other compilers"
#endif

    bool const result = pointsEqCompClose(c, d, relTol, absTol);

    if (!result) [[unlikely]] {
      TEST_MESSAGE_ALWAYS(std::format("Simple check: {}, "
                                      "PointMathBasic: {}",
                                      pointString(c), pointString(d)));
    }

    return result;
  };

  runRandomTests<3, Fp32>(rng, 500 /*numTrials*/, func);
  runRandomTests<3, Fp64>(rng, 500 /*numTrials*/, func);

  // MESSAGE_MAX_ERR_VARS
}
