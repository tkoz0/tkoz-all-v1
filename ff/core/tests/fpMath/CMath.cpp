#include <tkoz/ff/fpMath/CMath.hpp>
#include <tkoz/ff/fpMath/Numbers.hpp>

#include <tkoz/SRTest.hpp>

#include <tuple>

enum ErrType : uint8_t { ABS, REL };

// Helper for error bounds
template <typename T> struct FpErr {
  ErrType mErrType;
  T mErrVal;
  FpErr() = delete;
  FpErr(ErrType errType, T errVal) : mErrType(errType), mErrVal(errVal) {}
  inline void check(T actual, T expected) {
    switch (mErrType) {
    case ABS: {
      TEST_REQUIRE_NEAR_EQ_ABS(actual, expected, mErrVal);
    } break;
    case REL: {
      TEST_REQUIRE_NEAR_EQ_REL(actual, expected, mErrVal);
    } break;
    }
  }
};

// Run a test with sincos using both values and appropriate error bounds
[[maybe_unused]] auto doTest =
    []<typename T>(T arg, T sinExpected, T cosExpected, FpErr<T> errTestSin,
                   FpErr<T> errTestCos) {
      auto [sinActual, cosActual] = tkoz::ff::fpMath::cmathSinCos(arg);
      errTestSin.check(sinActual, sinExpected);
      errTestCos.check(cosActual, cosExpected);
    };

using tkoz::ff::fpMath::cmathSinCos;
using tkoz::ff::fpMath::cNumEps;
using tkoz::ff::fpMath::cNumPi;
using tkoz::ff::fpMath::cNumSqrt;

TEST_CREATE_FAST(cmathSinCosHalfpiMult) {
  // Note: use relative error near 1 and absolute error near 0
  static constexpr float tolFA = 10.0f * cNumEps<float>;  // float absolute
  static constexpr float tolFR = 10.0f * cNumEps<float>;  // float relative
  static constexpr double tolDA = 10.0 * cNumEps<double>; // double absolute
  static constexpr double tolDR = 10.0 * cNumEps<double>; // double relative
  static constexpr float piF = cNumPi<float>;
  static constexpr double piD = cNumPi<double>;
  { // 0
    auto const [sinF, cosF] = cmathSinCos(0.0f);
    auto const [sinD, cosD] = cmathSinCos(0.0);
    TEST_REQUIRE_NEAR_EQ_ABS(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, 1.0, tolDR);
  }
  { // pi/2
    auto [sinF, cosF] = cmathSinCos(piF / 2.0f);
    auto [sinD, cosD] = cmathSinCos(piD / 2.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(cosF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, 1.0, tolDR);
    TEST_REQUIRE_NEAR_EQ_ABS(cosD, 0.0, tolDA);
    // -pi/2
    std::tie(sinF, cosF) = cmathSinCos(-piF / 2.0f);
    std::tie(sinD, cosD) = cmathSinCos(-piD / 2.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(cosF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -1.0, tolDR);
    TEST_REQUIRE_NEAR_EQ_ABS(cosD, 0.0, tolDA);
  }
  { // pi
    auto [sinF, cosF] = cmathSinCos(piF);
    auto [sinD, cosD] = cmathSinCos(piD);
    TEST_REQUIRE_NEAR_EQ_ABS(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -1.0, tolDR);
    // -pi
    std::tie(sinF, cosF) = cmathSinCos(-piF);
    std::tie(sinD, cosD) = cmathSinCos(-piD);
    TEST_REQUIRE_NEAR_EQ_ABS(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -1.0, tolDR);
  }
  { // 3pi/2
    auto [sinF, cosF] = cmathSinCos(3.0f * piF / 2.0f);
    auto [sinD, cosD] = cmathSinCos(3.0 * piD / 2.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(cosF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -1.0, tolDR);
    TEST_REQUIRE_NEAR_EQ_ABS(cosD, 0.0, tolDA);
    // -3pi/2
    std::tie(sinF, cosF) = cmathSinCos(-3.0f * piF / 2.0f);
    std::tie(sinD, cosD) = cmathSinCos(-3.0 * piD / 2.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(cosF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, 1.0, tolDR);
    TEST_REQUIRE_NEAR_EQ_ABS(cosD, 0.0, tolDA);
  }
  { // 2pi
    auto [sinF, cosF] = cmathSinCos(2.0f * piF);
    auto [sinD, cosD] = cmathSinCos(2.0 * piD);
    TEST_REQUIRE_NEAR_EQ_ABS(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, 1.0, tolDR);
    // -2pi
    std::tie(sinF, cosF) = cmathSinCos(-2.0f * piF);
    std::tie(sinD, cosD) = cmathSinCos(-2.0 * piD);
    TEST_REQUIRE_NEAR_EQ_ABS(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR_EQ_ABS(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, 1.0, tolDR);
  }
}

TEST_CREATE_FAST(cmathSinCosThirdpiMult) {
  // Note: all values are not near 0 so only use relative error
  static constexpr float tolF = 10.0f * cNumEps<float>;  // float relative
  static constexpr double tolD = 10.0 * cNumEps<double>; // double relative
  static constexpr float piF = cNumPi<float>;
  static constexpr double piD = cNumPi<double>;
  static constexpr float r3F = cNumSqrt<float, 3> / 2.0f;
  static constexpr double r3D = cNumSqrt<double, 3> / 2.0f;
  { // pi/3
    auto [sinF, cosF] = cmathSinCos(piF / 3.0f);
    auto [sinD, cosD] = cmathSinCos(piD / 3.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, 0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, r3D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, 0.5, tolD);
    // -pi/3
    std::tie(sinF, cosF) = cmathSinCos(-piF / 3.0f);
    std::tie(sinD, cosD) = cmathSinCos(-piD / 3.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, 0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -r3D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, 0.5, tolD);
  }
  { // 2pi/3
    auto [sinF, cosF] = cmathSinCos(2.0f * piF / 3.0f);
    auto [sinD, cosD] = cmathSinCos(2.0 * piD / 3.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, r3D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -0.5, tolD);
    // -2pi/3
    std::tie(sinF, cosF) = cmathSinCos(-2.0f * piF / 3.0f);
    std::tie(sinD, cosD) = cmathSinCos(-2.0 * piD / 3.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -r3D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -0.5, tolD);
  }
  { // 4pi/3
    auto [sinF, cosF] = cmathSinCos(4.0f * piF / 3.0f);
    auto [sinD, cosD] = cmathSinCos(4.0 * piD / 3.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -r3D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -0.5, tolD);
    // -4pi/3
    std::tie(sinF, cosF) = cmathSinCos(-4.0f * piF / 3.0f);
    std::tie(sinD, cosD) = cmathSinCos(-4.0 * piD / 3.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, r3D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -0.5, tolD);
  }
  { // 5pi/3
    auto [sinF, cosF] = cmathSinCos(5.0f * piF / 3.0f);
    auto [sinD, cosD] = cmathSinCos(5.0 * piD / 3.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, 0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -r3D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, 0.5, tolD);
    // -5pi/3
    std::tie(sinF, cosF) = cmathSinCos(-5.0f * piF / 3.0f);
    std::tie(sinD, cosD) = cmathSinCos(-5.0 * piD / 3.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, 0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, r3D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, 0.5, tolD);
  }
}

TEST_CREATE_FAST(cmathSinCosFourthpiMult) {
  // Note: all values are not near 0 so only use relative error
  static constexpr float tolF = 10.0f * cNumEps<float>;  // float relative
  static constexpr double tolD = 10.0 * cNumEps<double>; // double relative
  static constexpr float piF = cNumPi<float>;
  static constexpr double piD = cNumPi<double>;
  static constexpr float r2F = cNumSqrt<float, 2> / 2.0f;
  static constexpr double r2D = cNumSqrt<double, 2> / 2.0f;
  { // pi/4
    auto [sinF, cosF] = cmathSinCos(piF / 4.0f);
    auto [sinD, cosD] = cmathSinCos(piD / 4.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, r2D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, r2D, tolD);
    // -pi/4
    std::tie(sinF, cosF) = cmathSinCos(-piF / 4.0f);
    std::tie(sinD, cosD) = cmathSinCos(-piD / 4.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -r2D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, r2D, tolD);
  }
  { // 3pi/4
    auto [sinF, cosF] = cmathSinCos(3.0f * piF / 4.0f);
    auto [sinD, cosD] = cmathSinCos(3.0 * piD / 4.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, r2D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -r2D, tolD);
    // -3pi/4
    std::tie(sinF, cosF) = cmathSinCos(-3.0f * piF / 4.0f);
    std::tie(sinD, cosD) = cmathSinCos(-3.0 * piD / 4.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -r2D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -r2D, tolD);
  }
  { // 5pi/4
    auto [sinF, cosF] = cmathSinCos(5.0f * piF / 4.0f);
    auto [sinD, cosD] = cmathSinCos(5.0 * piD / 4.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -r2D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -r2D, tolD);
    // -5pi/4
    std::tie(sinF, cosF) = cmathSinCos(-5.0f * piF / 4.0f);
    std::tie(sinD, cosD) = cmathSinCos(-5.0 * piD / 4.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, r2D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -r2D, tolD);
  }
  { // 7pi/4
    auto [sinF, cosF] = cmathSinCos(7.0f * piF / 4.0f);
    auto [sinD, cosD] = cmathSinCos(7.0 * piD / 4.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -r2D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, r2D, tolD);
    // -7pi/4
    std::tie(sinF, cosF) = cmathSinCos(-7.0f * piF / 4.0f);
    std::tie(sinD, cosD) = cmathSinCos(-7.0 * piD / 4.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, r2F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, r2D, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, r2D, tolD);
  }
}

TEST_CREATE_FAST(cmathSinCosSixthpiMult) {
  // Note: all values are not near 0 so only use relative error
  static constexpr float tolF = 10.0f * cNumEps<float>;  // float relative
  static constexpr double tolD = 10.0 * cNumEps<double>; // double relative
  static constexpr float piF = cNumPi<float>;
  static constexpr double piD = cNumPi<double>;
  static constexpr float r3F = cNumSqrt<float, 3> / 2.0f;
  static constexpr double r3D = cNumSqrt<double, 3> / 2.0f;
  { // pi/6
    auto [sinF, cosF] = cmathSinCos(piF / 6.0f);
    auto [sinD, cosD] = cmathSinCos(piD / 6.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, 0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, 0.5, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, r3D, tolD);
    // -pi/6
    std::tie(sinF, cosF) = cmathSinCos(-piF / 6.0f);
    std::tie(sinD, cosD) = cmathSinCos(-piD / 6.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -0.5, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, r3D, tolD);
  }
  { // 5pi/6
    auto [sinF, cosF] = cmathSinCos(5.0f * piF / 6.0f);
    auto [sinD, cosD] = cmathSinCos(5.0 * piD / 6.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, 0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, 0.5, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -r3D, tolD);
    // -5pi/6
    std::tie(sinF, cosF) = cmathSinCos(-5.0f * piF / 6.0f);
    std::tie(sinD, cosD) = cmathSinCos(-5.0 * piD / 6.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -0.5, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -r3D, tolD);
  }
  { // 7pi/6
    auto [sinF, cosF] = cmathSinCos(7.0f * piF / 6.0f);
    auto [sinD, cosD] = cmathSinCos(7.0 * piD / 6.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -0.5, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -r3D, tolD);
    // -7pi/6
    std::tie(sinF, cosF) = cmathSinCos(-7.0f * piF / 6.0f);
    std::tie(sinD, cosD) = cmathSinCos(-7.0 * piD / 6.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, 0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, -r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, 0.5, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, -r3D, tolD);
  }
  { // 11pi/6
    auto [sinF, cosF] = cmathSinCos(11.0f * piF / 6.0f);
    auto [sinD, cosD] = cmathSinCos(11.0 * piD / 6.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, -0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, -0.5, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, r3D, tolD);
    // -5pi/6
    std::tie(sinF, cosF) = cmathSinCos(-11.0f * piF / 6.0f);
    std::tie(sinD, cosD) = cmathSinCos(-11.0 * piD / 6.0);
    TEST_REQUIRE_NEAR_EQ_REL(sinF, 0.5f, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(cosF, r3F, tolF);
    TEST_REQUIRE_NEAR_EQ_REL(sinD, 0.5, tolD);
    TEST_REQUIRE_NEAR_EQ_REL(cosD, r3D, tolD);
  }
}
