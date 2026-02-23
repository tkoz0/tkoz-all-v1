#include <tkoz/ff/pointMath/PointMathBasic.hpp>

#include <tkoz/SRTest.hpp>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <random>

using tkoz::ff::PointData;
using tkoz::ff::PointMathBasic;
using tkoz::ff::fpMath::cNumEps;
using tkoz::ff::fpMath::cNumPi;
using tkoz::ff::fpMath::cNumPiMult;
using tkoz::ff::fpMath::cNumSqrt;

// Convert a float/double to string which is convertible back to exact value
template <typename T>
  requires std::is_same_v<T, float> || std::is_same_v<T, double>
inline auto fpString(T value) -> std::string {
  char buf[64];
  auto const [bufEnd, error] = std::to_chars(buf, buf + 64, value);
  TEST_REQUIRE_EQ(error, std::errc{});
  return std::string(buf, bufEnd);
}

// Create string representation of the point
template <std::size_t N, typename T>
inline auto pointString(PointData<N, T> const &value) -> std::string {
  std::string result;
  result.push_back('(');
  result += fpString(value.at(0));
  for (std::size_t j = 1; j < N; ++j) {
    result.push_back(',');
    result += fpString(value.at(j));
  }
  result.push_back(')');
  return result;
}

// Create an error message to show the actual point values used
template <std::size_t N, typename T>
  requires std::is_same_v<T, float> || std::is_same_v<T, double>
[[nodiscard]] auto errorMessageWithNumbers(PointData<N, T> const &actual,
                                           PointData<N, T> const &expected)
    -> std::string {
  return std::format("Expected {} but computed {} (type is {})",
                     pointString(expected), pointString(actual),
                     std::is_same_v<T, float> ? "float" : "double");
}

// Compare 2 points for equality by absolute error of corresponding components.
template <std::size_t N, typename T>
[[nodiscard]] auto pointsEqCompAbs(PointData<N, T> const &actual,
                                   PointData<N, T> const &expected, T err)
    -> bool {
  for (std::size_t i = 0; i < N; ++i) {
    if (std::abs(actual[i] - expected[i]) > err) [[unlikely]] {
      TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
      return false;
    }
  }
  return true;
}

// Compare 2 points for equality by relative error of corresponding components.
template <std::size_t N, typename T>
[[nodiscard]] auto pointsEqCompRel(PointData<N, T> const &actual,
                                   PointData<N, T> const &expected, T err)
    -> bool {
  for (std::size_t i = 0; i < N; ++i) {
    if (std::abs((actual[i] - expected[i]) / expected[i]) > err) [[unlikely]] {
      TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
      return false;
    }
  }
  return true;
}

// Compare 2 points for equality by max norm of their difference.
template <std::size_t N, typename T>
[[nodiscard]] auto pointsEqMax(PointData<N, T> const &actual,
                               PointData<N, T> const &expected, T err) -> bool {
  T maxComp{0.0};
  for (std::size_t i = 0; i < N; ++i) {
    maxComp = std::max(maxComp, std::abs(actual[i] - expected[i]));
  }
  return maxComp <= err;
}

// Compare 2 points for equality by L1 norm of their difference.
template <std::size_t N, typename T>
[[nodiscard]] auto pointsEqL1(PointData<N, T> const &actual,
                              PointData<N, T> const &expected, T err) -> bool {
  T compSum{0.0};
  for (std::size_t i = 0; i < N; ++i) {
    compSum += std::abs(actual[i] - expected[i]);
  }
  if (compSum > err) [[unlikely]] {
    TEST_MESSAGE_FAILURE(errorMessageWithNumbers(actual, expected));
  }
  return compSum <= err;
}

// Compare 2 points for equality by magnitude of their difference (euclidean).
template <std::size_t N, typename T>
[[nodiscard]] auto pointsEqL2(PointData<N, T> const &actual,
                              PointData<N, T> const &expected, T err) -> bool {
  T dotSum{0.0};
  for (std::size_t i = 0; i < N; ++i) {
    T const diff = actual[i] - expected[i];
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
    PointData<3, float> a(1.0f, 2.0f, 3.0f);
    PointData<3, float> const b(4.0f, 5.0f, 6.0f);
    PointMathBasic::addEq(a, b);
    PointData<3, float> const c(5.0f, 7.0f, 9.0f);
    TEST_REQUIRE(pointsEqCompRel(a, c, errF));
  }
  {
    PointData<4, double> a(3.1, 3.2, 3.3, -2.5);
    PointData<4, double> const b(1.7, 1.8, 1.9, -3.5);
    PointMathBasic::addEq(a, b);
    PointData<4, double> const c(4.8, 5.0, 5.2, -6.0);
    TEST_REQUIRE(pointsEqCompRel(a, c, errD));
  }
  {
    PointData<1, double> a(3.14);
    PointData<1, double> const b(-3.14);
    PointMathBasic::addEq(a, b);
    TEST_REQUIRE_EQ(a[0], 0.0);
  }
  {
    PointData<6, float> a(1.5f, -6.25f, 14.7f, -1e21, 5.7e23, -3.14f);
    PointData<6, float> const b(1e-10f, 1e-10f, -6.8f, -2e21, -4.6e23, 2.72f);
    PointMathBasic::addEq(a, b);
    PointData<6, float> const c(1.5f, -6.25f, 7.9f, -3e21, 1.1e23, -0.42f);
    TEST_REQUIRE(pointsEqCompRel(a, c, 10.0f * errF));
  }
  {
    PointData<2, double> a(2.0 + 5e-16, 3.0 - 8e-16);
    PointData<2, double> const b(-2.0, -3.0);
    PointMathBasic::addEq(a, b);
    PointData<2, double> const c(0.0, 0.0);
    TEST_REQUIRE(pointsEqCompAbs(a, c, errD));
  }
}

// Some manually created test cases for subtraction
TEST_CREATE_FAST(subEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<5, double> a(60.0, 50.0, 40.0, 30.0, 20.0);
    PointData<5, double> const b(70.0, 80.0, 90.0, 100.0, 110.0);
    PointMathBasic::subEq(a, b);
    PointData<5, double> const c(-10.0, -30.0, -50.0, -70.0, -90.0);
    TEST_REQUIRE(pointsEqCompRel(a, c, errD));
  }
  {
    PointData<1, float> a(1.0000001f);
    PointData<1, float> const b(1.0f);
    PointMathBasic::subEq(a, b);
    PointData<1, float> const c(0.0);
    TEST_REQUIRE(pointsEqCompAbs(a, c, errF));
  }
  {
    PointData<3, double> a(1.19, 2.21, 3.23);
    PointData<3, double> const b(-5.16, 2.73, 0.91);
    PointMathBasic::subEq(a, b);
    PointData<3, double> const c(6.35, -0.52, 2.32);
    TEST_REQUIRE(pointsEqCompRel(a, c, errD));
  }
  {
    PointData<2, float> a(1.0000001f, 0.9999999f);
    PointData<2, float> const b(1.0f, 1.0f);
    PointMathBasic::subEq(a, b);
    PointData<2, float> const c(0.0f, 0.0f);
    TEST_REQUIRE(pointsEqCompAbs(a, c, errF));
  }
}

TEST_CREATE_FAST(mulEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<3, double> a(1.4, 3.7, 5.9);
    PointMathBasic::mulEq(a, 3.0);
    PointData<3, double> const b(4.2, 11.1, 17.7);
    TEST_REQUIRE(pointsEqCompRel(a, b, errD));
  }
  {
    PointData<2, float> a(-3.6f, 6.3f);
    PointMathBasic::mulEq(a, -0.33333333f);
    PointData<2, float> const b(1.2f, -2.1f);
    TEST_REQUIRE(pointsEqCompRel(a, b, errF));
  }
}

TEST_CREATE_FAST(divEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<3, float> a(-1.5f, 1.2f, 2.0f);
    PointMathBasic::divEq(a, -0.2f);
    PointData<3, float> const b(7.5f, -6.0f, -10.0f);
    TEST_REQUIRE(pointsEqCompRel(a, b, errF));
  }
  {
    PointData<2, double> a(-1.65, 3.3);
    PointMathBasic::divEq(a, 1.1);
    PointData<2, double> const b(-1.5, 3.0);
    TEST_REQUIRE(pointsEqCompRel(a, b, errD));
  }
}

TEST_CREATE_FAST(compMulEqManual1) {
  // static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<7, double> a(-2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0);
    PointData<7, double> const b(-4.0, -3.0, -2.0, -1.0, 0.0, 1.0, 2.0);
    PointMathBasic::componentMulEq(a, b);
    PointData<7, double> const c(8.0, 3.0, 0.0, -1.0, 0.0, 3.0, 8.0);
    TEST_REQUIRE(pointsEqCompAbs(a, c, errD));
  }
}

TEST_CREATE_FAST(compDivEqManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  // static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<5, float> a(-2.0f, -1.0f, 0.0f, 1.0f, 2.0f);
    PointData<5, float> const b(1.0f, 2.0f, 3.0f, 4.0f, 5.0f);
    PointMathBasic::componentDivEq(a, b);
    PointData<5, float> const c(-2.0f, -0.5f, 0.0f, 0.25f, 0.4f);
    TEST_REQUIRE(pointsEqCompAbs(a, c, errF));
  }
}

TEST_CREATE_FAST(dotProductManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<4, double> const a(1.4, -3.5, 0.7, -0.3);
    PointData<4, double> const b(-1.6, -2.0, 1.0, 1.7);
    TEST_REQUIRE_NEAR_EQ_REL(PointMathBasic::dotProduct(a, b), 4.95, errD);
  }
  {
    PointData<3, float> const a(6.0f, -3.0f, -2.0f);
    PointData<3, float> const b(5.0f, 14.0f, -6.0f);
    TEST_REQUIRE_NEAR_EQ_ABS(PointMathBasic::dotProduct(a, b), 0.0f, errF);
  }
}

TEST_CREATE_FAST(angleBetweenManual1) {
  static constexpr float errF = 10.0f * cNumEps<float>;
  static constexpr double errD = 10.0 * cNumEps<double>;
  {
    PointData<6, float> const a(-6.0f, -8.0f, -143.0f, 54.0f, 41.0f, -86.0f);
    PointData<6, float> const b(6.0f, 8.0f, 143.0f, -54.0f, -41.0f, 86.0f);
    TEST_REQUIRE_NEAR_EQ_REL(PointMathBasic::angleBetween<false>(a, b),
                             cNumPi<float>, errF);
  }
  {
    PointData<4, double> const a(3.14, -7.958, -61.671, 106.1);
    PointData<4, double> const b(a);
    TEST_REQUIRE_NEAR_EQ_ABS(PointMathBasic::angleBetween<false>(a, b), 0.0,
                             errD);
  }
  {
    PointData<2, double> const a(-1.5, 0.5);
    PointData<2, double> const b(0.5, 1.5);
    TEST_REQUIRE_NEAR_EQ_REL(PointMathBasic::angleBetween<false>(a, b),
                             (cNumPiMult<double, 1, 2>), errD);
  }
  {
    PointData<3, float> const a(0.0f, 1.0f, 0.0f);
    PointData<3, float> const b(0.0f, 1.0f, cNumSqrt<float, 3>);
    TEST_REQUIRE_NEAR_EQ_REL(PointMathBasic::angleBetween<false>(a, b),
                             (cNumPiMult<float, 1, 3>), errF);
  }
}

// TEST_CREATE_FAST(cross2d) {}

// TEST_CREATE_FAST(cross3d) {}

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
void runRandomTests(std::mt19937_64 &rng, std::size_t numTrials, FuncT &&f) {
  // We will the components of the points with first something in [-1,1]
  // Then multiply by a random 2^k (k=0,1,...,7) which tests a wider range
  // Things like overflow/inf/nan still need to be tested separately
  std::uniform_real_distribution<NumT> unifDist(NumT{-1.0}, NumT{1.0});
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
        a[j] = unifDist(rng) * (1u << powDist(rng));
        b[j] = unifDist(rng) * (1u << powDist(rng));
      }
      result = std::forward<FuncT>(f).template operator()<cDimT, NumT>(a, b);
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
      NumT b = unifDist(rng) * (1u << powDist(rng));
      for (std::size_t j = 0; j < cDimT; ++j) {
        a[j] = unifDist(rng) * (1u << powDist(rng));
      }
      result = std::forward<FuncT>(f).template operator()<cDimT, NumT>(a, b);
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
        a[j] = unifDist(rng) * (1u << powDist(rng));
      }
      result = std::forward<FuncT>(f).template operator()<cDimT, NumT>(a);
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
                                   FuncT &&f) {
  TEST_MESSAGE_FAILURE(
      std::format("Running {} dimensions with float", cCurDimT));
  runRandomTests<cCurDimT, float>(rng, numTrials, std::forward<FuncT>(f));
  TEST_MESSAGE_FAILURE(
      std::format("Running {} dimensions with double", cCurDimT));
  runRandomTests<cCurDimT, double>(rng, numTrials, std::forward<FuncT>(f));
  if constexpr (cCurDimT < cMaxDimT) {
    runRandomTestsAllParamsHelper<cCurDimT + 1, cMaxDimT>(
        rng, numTrials, std::forward<FuncT>(f));
  }
}

template <std::size_t cMaxDimT, typename FuncT>
void runRandomTestsAllParams(std::size_t seed, std::size_t trials, FuncT &&f) {
  std::mt19937_64 rng(seed);
  runRandomTestsAllParamsHelper<1, cMaxDimT>(rng, trials,
                                             std::forward<FuncT>(f));
}

TEST_CREATE_FAST(addEqRandom1) {
  runRandomTestsAllParams<10>(
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
  runRandomTestsAllParams<10>(
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
  runRandomTestsAllParams<10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a, T b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] * b;
        }
        PointMathBasic::mulEq(a, b);
        // Should be exactly multiply parallel components
        return pointsEqExact(a, c);
      });
}

TEST_CREATE_FAST(divEqRandom1) {
  runRandomTestsAllParams<10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a, T b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] / b;
        }
        PointMathBasic::divEq(a, b);
        // We may actually multiply 1/b for performance so approx compare
        return pointsEqCompRel(a, c, cNumEps<T>);
      });
}

TEST_CREATE_FAST(compMulEqRandom1) {
  runRandomTestsAllParams<10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] * b[i];
        }
        PointMathBasic::componentMulEq(a, b);
        // Should be exactly subtract parallel components
        return pointsEqExact(a, c);
      });
}

TEST_CREATE_FAST(compDivEqRandom1) {
  runRandomTestsAllParams<10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        PointData<N, T> c;
        for (std::size_t i = 0; i < N; ++i) {
          c[i] = a[i] / b[i];
        }
        PointMathBasic::componentDivEq(a, b);
        // Should be exactly subtract parallel components
        return pointsEqExact(a, c);
      });
}

TEST_CREATE_FAST(dotProductRandom1) {
  runRandomTestsAllParams<10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        T c{0.0};
        for (std::size_t i = 0; i < N; ++i) {
          c += a[i] * b[i]; // Not using std::fma, should be nearly equal
        }
        T const d = PointMathBasic::dotProduct(a, b);
        // TODO use some relative error function
        // Notes with seed 42, 500 trials
        // - 5 have error over 100*eps
        // - we are checking a better fma dot product against basic summing
        // - should be enough to catch a wrong implementation,
        //   not enough to catch less stability than desired
        static constexpr T tol = T{700.0} * cNumEps<T>;
        // c is very unlikely to be zero in random testing so we ignore it
        T const err = std::abs((c - d) / c);
        if (err >= tol) [[unlikely]] {
          TEST_MESSAGE_ALWAYS(std::format("Simple no FMA for comparing: {}, "
                                          "PointMathBasic: {}, "
                                          "relative error: {}",
                                          fpString(c), fpString(d),
                                          fpString(err)));
        }
        return err < tol;
      });
}

TEST_CREATE_FAST(angleBetweenRandom1) {
  runRandomTestsAllParams<10>(
      42 /*seed*/, 500 /*trials*/,
      []<std::size_t N, typename T>(PointData<N, T> a,
                                    PointData<N, T> b) -> bool {
        // Test with basic acos method, but handle 1d separately
        if constexpr (N == 1) {
          TEST_REQUIRE(a.template at<0>() != T{0.0} &&
                       b.template at<0>() != T{0.0});
          T const c = PointMathBasic::angleBetween<false>(a, b);
          if (a.template at<0>() * b.template at<0>() < T{0.0}) {
            // Opposite direction
            return std::abs(c - cNumPi<T>) < T{1.0} * cNumEps<T>;
          } else {
            // Same direction
            return T{0.0} <= c && c < T{1.0} * cNumEps<T>;
          }
        } else {
          T aDotB{0.0};
          T aNorm{0.0};
          T bNorm{0.0};
          for (std::size_t i = 0; i < N; ++i) {
            // Not even using std::fma here
            aDotB += a[i] * b[i];
            aNorm += a[i] * a[i];
            bNorm += b[i] * b[i];
          }
          aNorm = std::sqrt(aNorm);
          bNorm = std::sqrt(bNorm);
          T const c =
              std::acos(std::clamp(aDotB / (aNorm * bNorm), T{-1.0}, T{1.0}));
          T const d = PointMathBasic::angleBetween<false>(a, b);
          // TODO use some relative error function
          // Notes with seed 42, 500 trials
          // - 45 have error over 100*eps (2 are angles near pi, all others 0)
          // - 10 have error over 1000*eps (all near 0)
          // - some errors get huge, we are checking a more stable
          //   (atan2 half angle) algorithm against a basic one with
          //   poor stability for angles near 0 or pi
          // - should be enough to catch a wrong implementation,
          //   not enough to catch less stability than desired
          static constexpr T tol = T{5400.0} * cNumEps<T>;
          T const err = std::abs((c - d) / c);
          if (err >= tol) [[unlikely]] {
            TEST_MESSAGE_ALWAYS(std::format("Simple acos for checking: {}, "
                                            "PointMathBasic: {}, "
                                            "relative error: {}",
                                            fpString(c), fpString(d),
                                            fpString(err)));
          }
          return err < tol;
        }
      });
}
