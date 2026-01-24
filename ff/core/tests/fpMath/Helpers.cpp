#include <tkoz/ff/fpMath/Helpers.hpp>

namespace tkoz::ff::fpMath {

static_assert(cFactorial<0> == 1ull);
static_assert(cFactorial<1> == 1ull);
static_assert(cFactorial<2> == 2ull);
static_assert(cFactorial<3> == 6ull);
static_assert(cFactorial<4> == 24ull);
static_assert(cFactorial<5> == 120ull);
static_assert(cFactorial<6> == 720ull);
static_assert(cFactorial<7> == 5040ull);
static_assert(cFactorial<8> == 40320ull);
static_assert(cFactorial<9> == 362880ull);
static_assert(cFactorial<10> == 3628800ull);
static_assert(cFactorial<11> == 39916800ull);
static_assert(cFactorial<12> == 479001600ull);
static_assert(cFactorial<13> == 6227020800ull);
static_assert(cFactorial<14> == 87178291200ull);
static_assert(cFactorial<15> == 1307674368000ull);
static_assert(cFactorial<16> == 20922789888000ull);
static_assert(cFactorial<17> == 355687428096000ull);
static_assert(cFactorial<18> == 6402373705728000ull);
static_assert(cFactorial<19> == 121645100408832000ull);
static_assert(cFactorial<20> == 2432902008176640000ull);

} // namespace tkoz::ff::fpMath
