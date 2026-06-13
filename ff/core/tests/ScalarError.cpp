#include <tkoz/ff/ScalarError.hpp>

#include <tkoz/SRTest.hpp>

static_assert(sizeof(tkoz::ff::internal::FpBits<float>::Signed) ==
              sizeof(float));
static_assert(sizeof(tkoz::ff::internal::FpBits<float>::Unsigned) ==
              sizeof(float));
static_assert(sizeof(tkoz::ff::internal::FpBits<double>::Signed) ==
              sizeof(double));
static_assert(sizeof(tkoz::ff::internal::FpBits<double>::Unsigned) ==
              sizeof(double));

TEST_CREATE_FAST(errAbs) {
  using tkoz::ff::errAbs;
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  TEST_REQUIRE_EQ(errAbs(Fp32{1.0f}, Fp32{1.5f}).value(), 0.5f);
  TEST_REQUIRE_EQ(errAbs(Fp64{0.5}, Fp64{-0.5}).value(), 1.0);
}

TEST_CREATE_FAST(errRel) {
  using tkoz::ff::errRel;
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  TEST_REQUIRE_EQ(errRel(Fp32{3.0f}, Fp32{4.0f}).value(), 0.25f);
  TEST_REQUIRE_EQ(errRel(Fp64{-2.0}, Fp64{0.5}).value(), 1.25);
}

TEST_CREATE_FAST(errUlp) {
  using tkoz::ff::errUlp;
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  TEST_REQUIRE_EQ(errUlp(Fp32{1.1f}, Fp32{1.0f}), 838861);
  TEST_REQUIRE_EQ(errUlp(Fp64{3.14}, Fp64{3.14}), 0);
}

TEST_CREATE_FAST(isCloseAbs) {
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  using tkoz::ff::isCloseAbs;
  TEST_REQUIRE(isCloseAbs(Fp32{1.6f}, Fp32{1.7f}, Fp32{0.2f}));
  TEST_REQUIRE(!isCloseAbs(Fp64{-0.3}, Fp64{-0.5}, Fp64{0.1}));
}

TEST_CREATE_FAST(isCloseRel) {
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  using tkoz::ff::isCloseRel;
  TEST_REQUIRE(isCloseRel(Fp32{15.0f}, Fp32{18.0f}, Fp32{0.2f}));
  TEST_REQUIRE(!isCloseRel(Fp64{1.7}, Fp64{1.8}, Fp64{0.05}));
}

TEST_CREATE_FAST(isCloseUlp) {
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  using tkoz::ff::isCloseUlp;
  TEST_REQUIRE(isCloseUlp(Fp32{1.01f}, Fp32{1.00f}, 100000));
  TEST_REQUIRE(!isCloseUlp(Fp32{1.01f}, Fp32{1.02f}, 50000));
}

TEST_CREATE_FAST(isClose) {
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  using tkoz::ff::isClose;
  TEST_REQUIRE(isClose(Fp32{7.1f}, Fp32{7.5f}, Fp32{0.001f}, Fp32{1.0f}));
  TEST_REQUIRE(!isClose(Fp64{-5.1}, Fp64{-5.3}, Fp64{0.01}, Fp64{0.1}));
}

TEST_CREATE_FAST(isNear) {
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  using tkoz::ff::isNear;
  TEST_REQUIRE(isNear(Fp32{1.3e9f}, Fp32{1.4e9f}, Fp32{0.1f}));
  TEST_REQUIRE(!isNear(Fp64{0.6}, Fp64{0.8}, Fp64{0.1}));
}

TEST_CREATE_FAST(isSame) {
  using tkoz::ff::Fp32;
  using tkoz::ff::Fp64;
  using tkoz::ff::isSame;
  TEST_REQUIRE(isSame(Fp32::nan(), Fp32::nan()));
  TEST_REQUIRE(!isSame(Fp64{-0.0}, Fp64{+0.0}));
  TEST_REQUIRE(isSame(Fp64{3.14}, Fp64{3.14}));
  TEST_REQUIRE(!isSame(Fp32{1.618f}, Fp32{2.718f}));
}
