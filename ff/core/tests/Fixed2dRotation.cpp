#include <tkoz/ff/Constants.hpp>
#include <tkoz/ff/Fixed2dRotation.hpp>
#include <tkoz/ff/Types.hpp>

#include <tkoz/SRTest.hpp>

using tkoz::ff::Fp32;
using tkoz::ff::Fp64;

TEST_CREATE_FAST(rotate1) {
  {
    auto rot = tkoz::ff::Fixed2dRotation(tkoz::ff::cNumPiMult<Fp32, 1, 2>);
    Fp32 x = 1.8f;
    Fp32 y = 1.8f;
    rot.apply(x, y);
    TEST_REQUIRE_NEAR(x.value(), -1.8f, 1e-7f);
    TEST_REQUIRE_NEAR(y.value(), 1.8f, 1e-7f);
  }
  {
    auto rot = tkoz::ff::Fixed2dRotation(tkoz::ff::cNumPi<Fp64>);
    auto [x, y] = rot.rotate(1.1, 1.9);
    TEST_REQUIRE_NEAR(x.value(), -1.1, 1e-15);
    TEST_REQUIRE_NEAR(y.value(), -1.9, 1e-15);
  }
}
