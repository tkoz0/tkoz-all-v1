#include <tkoz/ff/CMath.hpp>
#include <tkoz/ff/Constants.hpp>

#include <tkoz/SRTest.hpp>

#include <tuple>

using tkoz::ff::cmathSinCos;
using tkoz::ff::cNumEps;
using tkoz::ff::cNumPi;
using tkoz::ff::cNumSqrt;
using tkoz::ff::Fp32;
using tkoz::ff::Fp64;

TEST_CREATE_FAST(cmathSinCosHalfpiMult) {
  // Note: use relative error near 1 and absolute error near 0
  static constexpr float tolFA =
      10.0f * cNumEps<Fp32>.value(); // float absolute
  static constexpr float tolFR =
      10.0f * cNumEps<Fp32>.value(); // float relative
  static constexpr double tolDA =
      10.0 * cNumEps<Fp64>.value(); // double absolute
  static constexpr double tolDR =
      10.0 * cNumEps<Fp64>.value(); // double relative
  static constexpr float piF = cNumPi<Fp32>.value();
  static constexpr double piD = cNumPi<Fp64>.value();
  { // 0
    auto const [sinF, cosF] = cmathSinCos(0.0f);
    auto const [sinD, cosD] = cmathSinCos(0.0);
    TEST_REQUIRE_NEAR(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(cosF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR(cosD, 1.0, tolDR);
  }
  { // pi/2
    auto [sinF, cosF] = cmathSinCos(piF / 2.0f);
    auto [sinD, cosD] = cmathSinCos(piD / 2.0);
    TEST_REQUIRE_NEAR(sinF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR(cosF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(sinD, 1.0, tolDR);
    TEST_REQUIRE_NEAR(cosD, 0.0, tolDA);
    // -pi/2
    std::tie(sinF, cosF) = cmathSinCos(-piF / 2.0f);
    std::tie(sinD, cosD) = cmathSinCos(-piD / 2.0);
    TEST_REQUIRE_NEAR(sinF, -1.0f, tolFR);
    TEST_REQUIRE_NEAR(cosF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(sinD, -1.0, tolDR);
    TEST_REQUIRE_NEAR(cosD, 0.0, tolDA);
  }
  { // pi
    auto [sinF, cosF] = cmathSinCos(piF);
    auto [sinD, cosD] = cmathSinCos(piD);
    TEST_REQUIRE_NEAR(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(cosF, -1.0f, tolFR);
    TEST_REQUIRE_NEAR(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR(cosD, -1.0, tolDR);
    // -pi
    std::tie(sinF, cosF) = cmathSinCos(-piF);
    std::tie(sinD, cosD) = cmathSinCos(-piD);
    TEST_REQUIRE_NEAR(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(cosF, -1.0f, tolFR);
    TEST_REQUIRE_NEAR(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR(cosD, -1.0, tolDR);
  }
  { // 3pi/2
    auto [sinF, cosF] = cmathSinCos(3.0f * piF / 2.0f);
    auto [sinD, cosD] = cmathSinCos(3.0 * piD / 2.0);
    TEST_REQUIRE_NEAR(sinF, -1.0f, tolFR);
    TEST_REQUIRE_NEAR(cosF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(sinD, -1.0, tolDR);
    TEST_REQUIRE_NEAR(cosD, 0.0, tolDA);
    // -3pi/2
    std::tie(sinF, cosF) = cmathSinCos(-3.0f * piF / 2.0f);
    std::tie(sinD, cosD) = cmathSinCos(-3.0 * piD / 2.0);
    TEST_REQUIRE_NEAR(sinF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR(cosF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(sinD, 1.0, tolDR);
    TEST_REQUIRE_NEAR(cosD, 0.0, tolDA);
  }
  { // 2pi
    auto [sinF, cosF] = cmathSinCos(2.0f * piF);
    auto [sinD, cosD] = cmathSinCos(2.0 * piD);
    TEST_REQUIRE_NEAR(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(cosF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR(cosD, 1.0, tolDR);
    // -2pi
    std::tie(sinF, cosF) = cmathSinCos(-2.0f * piF);
    std::tie(sinD, cosD) = cmathSinCos(-2.0 * piD);
    TEST_REQUIRE_NEAR(sinF, 0.0f, tolFA);
    TEST_REQUIRE_NEAR(cosF, 1.0f, tolFR);
    TEST_REQUIRE_NEAR(sinD, 0.0, tolDA);
    TEST_REQUIRE_NEAR(cosD, 1.0, tolDR);
  }
}

TEST_CREATE_FAST(cmathSinCosThirdpiMult) {
  // Note: all values are not near 0 so only use relative error
  static constexpr float tolF = 10.0f * cNumEps<Fp32>.value(); // float relative
  static constexpr double tolD =
      10.0 * cNumEps<Fp64>.value(); // double relative
  static constexpr float piF = cNumPi<Fp32>.value();
  static constexpr double piD = cNumPi<Fp64>.value();
  static constexpr float r3F = cNumSqrt<Fp32, 3>.value() / 2.0f;
  static constexpr double r3D = cNumSqrt<Fp64, 3>.value() / 2.0;
  { // pi/3
    auto [sinF, cosF] = cmathSinCos(piF / 3.0f);
    auto [sinD, cosD] = cmathSinCos(piD / 3.0);
    TEST_REQUIRE_NEAR(sinF, r3F, tolF);
    TEST_REQUIRE_NEAR(cosF, 0.5f, tolF);
    TEST_REQUIRE_NEAR(sinD, r3D, tolD);
    TEST_REQUIRE_NEAR(cosD, 0.5, tolD);
    // -pi/3
    std::tie(sinF, cosF) = cmathSinCos(-piF / 3.0f);
    std::tie(sinD, cosD) = cmathSinCos(-piD / 3.0);
    TEST_REQUIRE_NEAR(sinF, -r3F, tolF);
    TEST_REQUIRE_NEAR(cosF, 0.5f, tolF);
    TEST_REQUIRE_NEAR(sinD, -r3D, tolD);
    TEST_REQUIRE_NEAR(cosD, 0.5, tolD);
  }
  { // 2pi/3
    auto [sinF, cosF] = cmathSinCos(2.0f * piF / 3.0f);
    auto [sinD, cosD] = cmathSinCos(2.0 * piD / 3.0);
    TEST_REQUIRE_NEAR(sinF, r3F, tolF);
    TEST_REQUIRE_NEAR(cosF, -0.5f, tolF);
    TEST_REQUIRE_NEAR(sinD, r3D, tolD);
    TEST_REQUIRE_NEAR(cosD, -0.5, tolD);
    // -2pi/3
    std::tie(sinF, cosF) = cmathSinCos(-2.0f * piF / 3.0f);
    std::tie(sinD, cosD) = cmathSinCos(-2.0 * piD / 3.0);
    TEST_REQUIRE_NEAR(sinF, -r3F, tolF);
    TEST_REQUIRE_NEAR(cosF, -0.5f, tolF);
    TEST_REQUIRE_NEAR(sinD, -r3D, tolD);
    TEST_REQUIRE_NEAR(cosD, -0.5, tolD);
  }
  { // 4pi/3
    auto [sinF, cosF] = cmathSinCos(4.0f * piF / 3.0f);
    auto [sinD, cosD] = cmathSinCos(4.0 * piD / 3.0);
    TEST_REQUIRE_NEAR(sinF, -r3F, tolF);
    TEST_REQUIRE_NEAR(cosF, -0.5f, tolF);
    TEST_REQUIRE_NEAR(sinD, -r3D, tolD);
    TEST_REQUIRE_NEAR(cosD, -0.5, tolD);
    // -4pi/3
    std::tie(sinF, cosF) = cmathSinCos(-4.0f * piF / 3.0f);
    std::tie(sinD, cosD) = cmathSinCos(-4.0 * piD / 3.0);
    TEST_REQUIRE_NEAR(sinF, r3F, tolF);
    TEST_REQUIRE_NEAR(cosF, -0.5f, tolF);
    TEST_REQUIRE_NEAR(sinD, r3D, tolD);
    TEST_REQUIRE_NEAR(cosD, -0.5, tolD);
  }
  { // 5pi/3
    auto [sinF, cosF] = cmathSinCos(5.0f * piF / 3.0f);
    auto [sinD, cosD] = cmathSinCos(5.0 * piD / 3.0);
    TEST_REQUIRE_NEAR(sinF, -r3F, tolF);
    TEST_REQUIRE_NEAR(cosF, 0.5f, tolF);
    TEST_REQUIRE_NEAR(sinD, -r3D, tolD);
    TEST_REQUIRE_NEAR(cosD, 0.5, tolD);
    // -5pi/3
    std::tie(sinF, cosF) = cmathSinCos(-5.0f * piF / 3.0f);
    std::tie(sinD, cosD) = cmathSinCos(-5.0 * piD / 3.0);
    TEST_REQUIRE_NEAR(sinF, r3F, tolF);
    TEST_REQUIRE_NEAR(cosF, 0.5f, tolF);
    TEST_REQUIRE_NEAR(sinD, r3D, tolD);
    TEST_REQUIRE_NEAR(cosD, 0.5, tolD);
  }
}

TEST_CREATE_FAST(cmathSinCosFourthpiMult) {
  // Note: all values are not near 0 so only use relative error
  static constexpr float tolF = 10.0f * cNumEps<Fp32>.value(); // float relative
  static constexpr double tolD =
      10.0 * cNumEps<Fp64>.value(); // double relative
  static constexpr float piF = cNumPi<Fp32>.value();
  static constexpr double piD = cNumPi<Fp64>.value();
  static constexpr float r2F = cNumSqrt<Fp32, 2>.value() / 2.0f;
  static constexpr double r2D = cNumSqrt<Fp64, 2>.value() / 2.0;
  { // pi/4
    auto [sinF, cosF] = cmathSinCos(piF / 4.0f);
    auto [sinD, cosD] = cmathSinCos(piD / 4.0);
    TEST_REQUIRE_NEAR(sinF, r2F, tolF);
    TEST_REQUIRE_NEAR(cosF, r2F, tolF);
    TEST_REQUIRE_NEAR(sinD, r2D, tolD);
    TEST_REQUIRE_NEAR(cosD, r2D, tolD);
    // -pi/4
    std::tie(sinF, cosF) = cmathSinCos(-piF / 4.0f);
    std::tie(sinD, cosD) = cmathSinCos(-piD / 4.0);
    TEST_REQUIRE_NEAR(sinF, -r2F, tolF);
    TEST_REQUIRE_NEAR(cosF, r2F, tolF);
    TEST_REQUIRE_NEAR(sinD, -r2D, tolD);
    TEST_REQUIRE_NEAR(cosD, r2D, tolD);
  }
  { // 3pi/4
    auto [sinF, cosF] = cmathSinCos(3.0f * piF / 4.0f);
    auto [sinD, cosD] = cmathSinCos(3.0 * piD / 4.0);
    TEST_REQUIRE_NEAR(sinF, r2F, tolF);
    TEST_REQUIRE_NEAR(cosF, -r2F, tolF);
    TEST_REQUIRE_NEAR(sinD, r2D, tolD);
    TEST_REQUIRE_NEAR(cosD, -r2D, tolD);
    // -3pi/4
    std::tie(sinF, cosF) = cmathSinCos(-3.0f * piF / 4.0f);
    std::tie(sinD, cosD) = cmathSinCos(-3.0 * piD / 4.0);
    TEST_REQUIRE_NEAR(sinF, -r2F, tolF);
    TEST_REQUIRE_NEAR(cosF, -r2F, tolF);
    TEST_REQUIRE_NEAR(sinD, -r2D, tolD);
    TEST_REQUIRE_NEAR(cosD, -r2D, tolD);
  }
  { // 5pi/4
    auto [sinF, cosF] = cmathSinCos(5.0f * piF / 4.0f);
    auto [sinD, cosD] = cmathSinCos(5.0 * piD / 4.0);
    TEST_REQUIRE_NEAR(sinF, -r2F, tolF);
    TEST_REQUIRE_NEAR(cosF, -r2F, tolF);
    TEST_REQUIRE_NEAR(sinD, -r2D, tolD);
    TEST_REQUIRE_NEAR(cosD, -r2D, tolD);
    // -5pi/4
    std::tie(sinF, cosF) = cmathSinCos(-5.0f * piF / 4.0f);
    std::tie(sinD, cosD) = cmathSinCos(-5.0 * piD / 4.0);
    TEST_REQUIRE_NEAR(sinF, r2F, tolF);
    TEST_REQUIRE_NEAR(cosF, -r2F, tolF);
    TEST_REQUIRE_NEAR(sinD, r2D, tolD);
    TEST_REQUIRE_NEAR(cosD, -r2D, tolD);
  }
  { // 7pi/4
    auto [sinF, cosF] = cmathSinCos(7.0f * piF / 4.0f);
    auto [sinD, cosD] = cmathSinCos(7.0 * piD / 4.0);
    TEST_REQUIRE_NEAR(sinF, -r2F, tolF);
    TEST_REQUIRE_NEAR(cosF, r2F, tolF);
    TEST_REQUIRE_NEAR(sinD, -r2D, tolD);
    TEST_REQUIRE_NEAR(cosD, r2D, tolD);
    // -7pi/4
    std::tie(sinF, cosF) = cmathSinCos(-7.0f * piF / 4.0f);
    std::tie(sinD, cosD) = cmathSinCos(-7.0 * piD / 4.0);
    TEST_REQUIRE_NEAR(sinF, r2F, tolF);
    TEST_REQUIRE_NEAR(cosF, r2F, tolF);
    TEST_REQUIRE_NEAR(sinD, r2D, tolD);
    TEST_REQUIRE_NEAR(cosD, r2D, tolD);
  }
}

TEST_CREATE_FAST(cmathSinCosSixthpiMult) {
  // Note: all values are not near 0 so only use relative error
  static constexpr float tolF = 10.0f * cNumEps<Fp32>.value(); // float relative
  static constexpr double tolD =
      10.0 * cNumEps<Fp64>.value(); // double relative
  static constexpr float piF = cNumPi<Fp32>.value();
  static constexpr double piD = cNumPi<Fp64>.value();
  static constexpr float r3F = cNumSqrt<Fp32, 3>.value() / 2.0f;
  static constexpr double r3D = cNumSqrt<Fp64, 3>.value() / 2.0;
  { // pi/6
    auto [sinF, cosF] = cmathSinCos(piF / 6.0f);
    auto [sinD, cosD] = cmathSinCos(piD / 6.0);
    TEST_REQUIRE_NEAR(sinF, 0.5f, tolF);
    TEST_REQUIRE_NEAR(cosF, r3F, tolF);
    TEST_REQUIRE_NEAR(sinD, 0.5, tolD);
    TEST_REQUIRE_NEAR(cosD, r3D, tolD);
    // -pi/6
    std::tie(sinF, cosF) = cmathSinCos(-piF / 6.0f);
    std::tie(sinD, cosD) = cmathSinCos(-piD / 6.0);
    TEST_REQUIRE_NEAR(sinF, -0.5f, tolF);
    TEST_REQUIRE_NEAR(cosF, r3F, tolF);
    TEST_REQUIRE_NEAR(sinD, -0.5, tolD);
    TEST_REQUIRE_NEAR(cosD, r3D, tolD);
  }
  { // 5pi/6
    auto [sinF, cosF] = cmathSinCos(5.0f * piF / 6.0f);
    auto [sinD, cosD] = cmathSinCos(5.0 * piD / 6.0);
    TEST_REQUIRE_NEAR(sinF, 0.5f, tolF);
    TEST_REQUIRE_NEAR(cosF, -r3F, tolF);
    TEST_REQUIRE_NEAR(sinD, 0.5, tolD);
    TEST_REQUIRE_NEAR(cosD, -r3D, tolD);
    // -5pi/6
    std::tie(sinF, cosF) = cmathSinCos(-5.0f * piF / 6.0f);
    std::tie(sinD, cosD) = cmathSinCos(-5.0 * piD / 6.0);
    TEST_REQUIRE_NEAR(sinF, -0.5f, tolF);
    TEST_REQUIRE_NEAR(cosF, -r3F, tolF);
    TEST_REQUIRE_NEAR(sinD, -0.5, tolD);
    TEST_REQUIRE_NEAR(cosD, -r3D, tolD);
  }
  { // 7pi/6
    auto [sinF, cosF] = cmathSinCos(7.0f * piF / 6.0f);
    auto [sinD, cosD] = cmathSinCos(7.0 * piD / 6.0);
    TEST_REQUIRE_NEAR(sinF, -0.5f, tolF);
    TEST_REQUIRE_NEAR(cosF, -r3F, tolF);
    TEST_REQUIRE_NEAR(sinD, -0.5, tolD);
    TEST_REQUIRE_NEAR(cosD, -r3D, tolD);
    // -7pi/6
    std::tie(sinF, cosF) = cmathSinCos(-7.0f * piF / 6.0f);
    std::tie(sinD, cosD) = cmathSinCos(-7.0 * piD / 6.0);
    TEST_REQUIRE_NEAR(sinF, 0.5f, tolF);
    TEST_REQUIRE_NEAR(cosF, -r3F, tolF);
    TEST_REQUIRE_NEAR(sinD, 0.5, tolD);
    TEST_REQUIRE_NEAR(cosD, -r3D, tolD);
  }
  { // 11pi/6
    auto [sinF, cosF] = cmathSinCos(11.0f * piF / 6.0f);
    auto [sinD, cosD] = cmathSinCos(11.0 * piD / 6.0);
    TEST_REQUIRE_NEAR(sinF, -0.5f, tolF);
    TEST_REQUIRE_NEAR(cosF, r3F, tolF);
    TEST_REQUIRE_NEAR(sinD, -0.5, tolD);
    TEST_REQUIRE_NEAR(cosD, r3D, tolD);
    // -5pi/6
    std::tie(sinF, cosF) = cmathSinCos(-11.0f * piF / 6.0f);
    std::tie(sinD, cosD) = cmathSinCos(-11.0 * piD / 6.0);
    TEST_REQUIRE_NEAR(sinF, 0.5f, tolF);
    TEST_REQUIRE_NEAR(cosF, r3F, tolF);
    TEST_REQUIRE_NEAR(sinD, 0.5, tolD);
    TEST_REQUIRE_NEAR(cosD, r3D, tolD);
  }
}
