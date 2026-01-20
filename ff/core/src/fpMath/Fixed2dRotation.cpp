#include <tkoz/ff/fpMath/Fixed2dRotation.hpp>

#if TEST

#include <tkoz/ff/fpMath/Numbers.hpp>

#include <tkoz/SRTest.hpp>

TEST_CREATE_FAST(rotate1) {
  {
    auto rot = tkoz::ff::fpMath::Fixed2dRotation(
        tkoz::ff::fpMath::cNumPiMult<float, 1, 2>);
    float x = 1.8f;
    float y = 1.8f;
    rot.apply(x, y);
    TEST_REQUIRE_NEAR_EQ_REL(x, -1.8f, 1e-7f);
    TEST_REQUIRE_NEAR_EQ_REL(y, 1.8f, 1e-7f);
  }
  {
    auto rot =
        tkoz::ff::fpMath::Fixed2dRotation(tkoz::ff::fpMath::cNumPi<double>);
    auto [x, y] = rot.rotate(1.1, 1.9);
    TEST_REQUIRE_NEAR_EQ_REL(x, -1.1, 1e-15);
    TEST_REQUIRE_NEAR_EQ_REL(y, -1.9, 1e-15);
  }
}

#endif // TEST
