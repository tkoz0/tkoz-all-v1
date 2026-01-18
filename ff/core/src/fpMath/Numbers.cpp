#include <tkoz/ff/fpMath/Numbers.hpp>

#if TEST

#include <cfloat>
#include <limits>
#include <numbers>

// Significant digits of precision that can be represented exactly without
// changing the provided value. One extra digit past this may be needed for
// fully reversible conversion.
static_assert(std::numeric_limits<float>::digits10 == 6);
static_assert(std::numeric_limits<double>::digits10 == 15);
static_assert(FLT_DIG == 6);
static_assert(DBL_DIG == 15);

// Binary mantissa sizes (including implicit 1)
static_assert(FLT_MANT_DIG == 24);
static_assert(DBL_MANT_DIG == 53);

// Machine epsilon
static_assert(FLT_EPSILON == 1.19209290e-7f);
static_assert(DBL_EPSILON == 2.2204460492503131e-16);

namespace tkoz::ff {

// Test the machine epsilon
static_assert(cNumEps<float> == std::numeric_limits<float>::epsilon());
static_assert(cNumEps<double> == std::numeric_limits<double>::epsilon());
static_assert(1.0f + cNumEps<float> > 1.0f);
static_assert(1.0 + cNumEps<double> > 1.0);
static_assert(1.0f + (cNumEps<float> / 2.0f) == 1.0f);
static_assert(1.0 + (cNumEps<double> / 2.0) == 1.0);
static_assert(2.0f + cNumEps<float> == 2.0f);
static_assert(2.0 + cNumEps<double> == 2.0);
static_assert(2.0f - cNumEps<float> < 2.0f);
static_assert(2.0 - cNumEps<double> < 2.0);

///
/// e = 2.7182818284 5904523536 0287471352 6624977572 4709369995
///

// Shortest exact literals
static_assert(cNumE<float> == 2.7182817f);
static_assert(cNumE<float> == 2.7182818f); // use this
static_assert(cNumE<double> == 2.718281828459045);
// Check against STL
static_assert(cNumE<float> == std::numbers::e_v<float>);
static_assert(cNumE<double> == std::numbers::e_v<double>);

// float (8)
static_assert(cNumE<float> != 2.7182816f);
static_assert(cNumE<float> == 2.7182817f);
static_assert(cNumE<float> == 2.7182818f); // use this
static_assert(cNumE<float> != 2.7182819f);

// float (9)
static_assert(cNumE<float> != 2.71828162f);
static_assert(cNumE<float> == 2.71828163f);
static_assert(cNumE<float> == 2.71828164f);
// ...
static_assert(cNumE<float> == 2.71828182f);
static_assert(cNumE<float> == 2.71828183f); // use this
static_assert(cNumE<float> == 2.71828184f);
static_assert(cNumE<float> == 2.71828185f);
static_assert(cNumE<float> == 2.71828186f);
static_assert(cNumE<float> != 2.71828187f);

// double (16)
static_assert(cNumE<double> != 2.718281828459044);
static_assert(cNumE<double> == 2.718281828459045); // use this
static_assert(cNumE<double> != 2.718281828459046);

// double (17)
static_assert(cNumE<double> != 2.7182818284590448);
static_assert(cNumE<double> == 2.7182818284590449);
static_assert(cNumE<double> == 2.7182818284590450);
static_assert(cNumE<double> == 2.7182818284590451);
static_assert(cNumE<double> == 2.7182818284590452); // use this
static_assert(cNumE<double> == 2.7182818284590453);
static_assert(cNumE<double> != 2.7182818284590454);

///
/// pi = 3.1415926535 8979323846 2643383279 5028841971 6939937510
///

// Shortest exact literals
static_assert(cNumPi<float> == 3.1415927f); // use this
static_assert(cNumPi<float> == 3.1415928f);
static_assert(cNumPi<double> == 3.141592653589793);
// Check against STL
static_assert(cNumPi<float> == std::numbers::pi_v<float>);
static_assert(cNumPi<double> == std::numbers::pi_v<double>);

// float (8)
static_assert(cNumPi<float> != 3.1415926f);
static_assert(cNumPi<float> == 3.1415927f); // use this
static_assert(cNumPi<float> == 3.1415928f);
static_assert(cNumPi<float> != 3.1415929f);

// float (9)
static_assert(cNumPi<float> != 3.14159262f);
static_assert(cNumPi<float> == 3.14159263f);
static_assert(cNumPi<float> == 3.14159264f);
static_assert(cNumPi<float> == 3.14159265f); // use this
static_assert(cNumPi<float> == 3.14159266f);
static_assert(cNumPi<float> == 3.14159267f);
// ...
static_assert(cNumPi<float> == 3.14159285f);
static_assert(cNumPi<float> == 3.14159286f);
static_assert(cNumPi<float> != 3.14159287f);

// double (16)
static_assert(cNumPi<double> != 3.141592653589792);
static_assert(cNumPi<double> == 3.141592653589793); // use this
static_assert(cNumPi<double> != 3.141592653589794);

// double (17)
static_assert(cNumPi<double> != 3.1415926535897928);
static_assert(cNumPi<double> == 3.1415926535897929);
static_assert(cNumPi<double> == 3.1415926535897930);
static_assert(cNumPi<double> == 3.1415926535897931);
static_assert(cNumPi<double> == 3.1415926535897932); // use this
static_assert(cNumPi<double> == 3.1415926535897933);
static_assert(cNumPi<double> != 3.1415926535897934);

// TODO possible constants to add
// constants for memes (+ templated one)
// - cNumZero
// - cNumOneHalf
// - cNumThreeHalfs
// - cNumFiveHalfs
// - cNumOneThird
// - cNumTwoThirds
// - cNumFourThirds
// - cNumOneFourth
// - cNumTwoFourths
// - cNumOneFifth
// - cNumTwoFifths
// - cNumThreeFifths
// - cNumFourFifths
// - cNumOneSixth
// - cNumFiveSixths
// - cNumOne
// - cNumTwo
// - cNumThree
// - cNumFour
// - cNumFive
// machine epsilon
// - cNumEps = (todo)
// e
// - cNumE = e
// - cNum1DivE = 1/e
// pi multiples (+ templated one)
// - cNumPi
// - cNum2Pi = 2*pi
// - cNumPiDiv2 = pi/2
// - cNumPiDiv3 = pi/3
// - cNum2PiDiv3 = 2*pi/3
// - cNumPiDiv4 = pi/4
// - cNum3PiDiv4 = 3*pi/4
// - cNumPiDiv6 = pi/6
// - cNum5PiDiv6 = 5*pi/6
// pi inverse multiples
// - cNum1DivPi = 1/pi
// - cNum1Div2Pi = 1/(2*pi)
// - cNum2DivPi = 2/pi
// pi root
// - cNumRootPi = sqrt(pi)
// - cNum1DivRootPi = 1/sqrt(pi)
// sqrt based
// - cNumPhi = phi = (1+sqrt(5))/2
// - cNumSqrt2 = sqrt(2)
// - cNumSqrt3 = sqrt(3)
// - cNumSqrt5 = sqrt(5)
// inv sqrt based
// - cNum1DivPhi = 1/phi
// - cNum1DivSqrt2 = 1/sqrt(2)
// - cNum1DivSqrt3 = 1/sqrt(3)
// - cNum1DivSqrt5 = 1/sqrt(5)
// advanced
// - cNumEulerGamma = gamma
// log based
// - cNumLog2 = log(2)
// - cNumLog3 = log(3)
// - cNumLog10 = log(10)
// - log2(e)
// - log10(e)
// other ...

} // namespace tkoz::ff

#endif // TEST
