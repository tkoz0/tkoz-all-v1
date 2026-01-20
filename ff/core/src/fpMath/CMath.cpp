#include <tkoz/ff/fpMath/CMath.hpp>

#if TEST

#include <tkoz/ff/fpMath/Numbers.hpp>

#include <tkoz/SRTest.hpp>

TEST_CREATE_FAST(sincos1) {
  { // 0
    auto [s, c] = tkoz::ff::fpMath::cmathSinCos(0.0);
    TEST_REQUIRE_NEAR_EQ_ABS(s, 0.0, 1e-15);
    TEST_REQUIRE_NEAR_EQ_REL(c, 1.0, 1e-15);
  }
  { // pi/2
    auto [s, c] = tkoz::ff::fpMath::cmathSinCos(
        tkoz::ff::fpMath::cNumPiMult<double, 1, 2>);
    TEST_REQUIRE_NEAR_EQ_REL(s, 1.0, 1e-15);
    TEST_REQUIRE_NEAR_EQ_ABS(c, 0.0, 1e-15);
  }
  { // pi
    auto [s, c] =
        tkoz::ff::fpMath::cmathSinCos(tkoz::ff::fpMath::cNumPi<double>);
    TEST_REQUIRE_NEAR_EQ_ABS(s, 0.0, 1e-15);
    TEST_REQUIRE_NEAR_EQ_REL(c, -1.0, 1e-15);
  }
}

#endif // TEST
