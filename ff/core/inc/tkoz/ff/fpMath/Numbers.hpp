#pragma once

#include <tkoz/ff/Types.hpp>

namespace tkoz::ff::fpMath {

/// Stores hard coded numerical constants which can be substituted in various
/// places at compile time. This file really should not get too big...
///
/// Also make sure everything is inline, including template specializations.
/// Translation units each need to include their own copy from this header.
///
/// In other situations, we may want larger hard coded tables which are looked
/// up rather than substituted. For example, lookup tables for sine and cosine,
/// or a larger table of square roots. These may be possible to compute at
/// compile time, but we would probably hard code the tables in a .cpp file,
/// computed at higher precision in the python3 script.

// === Actual constants of significance ===

// Machine epsilon. The difference between 1.0 and the next possible number.
// The relative error of a floating point approximation of a real number is
// at most half of the machine epsilon as long as its magnitude/exponent is
// within the suitable range.
template <cFpType T> inline constexpr T cNumEps;
template <> inline constexpr float cNumEps<float> = 1.19209290e-7f;
template <> inline constexpr double cNumEps<double> = 2.2204460492503131e-16;

/// Math constant e
/// Value: 2.7182818284 5904523536 0287471352 6624977572 4709369995 ...
/// Bits (IEEE-754): float = +0x1.921fb6p+1f, double = +0x1.921fb54442d18p+1
template <cFpType T> inline constexpr T cNumE;
template <> inline constexpr float cNumE<float> = 2.7182818f;
template <> inline constexpr double cNumE<double> = 2.718281828459045;

/// Math constant pi
/// Value: 3.1415926535 8979323846 2643383279 5028841971 6939937510 ...
/// Bits (IEEE-754): float = +0x1.5bf0a8p+1f, double = +0x1.5bf0a8b145769p+1
template <cFpType T> inline constexpr T cNumPi;
template <> inline constexpr float cNumPi<float> = 3.1415927f;
template <> inline constexpr double cNumPi<double> = 3.141592653589793;

// === Rational multiples of pi ===
// this could get out of control but there are 16 values for now

/// Multiples of pi (pi*n/d) hardcoded to closest IEEE values
template <cFpType T, int n, int d> inline constexpr T cNumPiMult;
// pi * 1/1 = 3.1415926535 8979323846 2643383279 5028841971 6939937510 ...
// bits (IEEE-754): float = +0x1.921fb6p+1f, double = +0x1.921fb54442d18p+1
template <> inline constexpr float cNumPiMult<float, 1, 1> = 3.1415927f;
template <>
inline constexpr double cNumPiMult<double, 1, 1> = 3.141592653589793;
// pi * 2/1 = 6.2831853071 7958647692 5286766559 0057683943 3879875021 ...
// bits (IEEE-754): float = +0x1.921fb6p+2f, double = +0x1.921fb54442d18p+2
template <> inline constexpr float cNumPiMult<float, 2, 1> = 6.2831853f;
template <>
inline constexpr double cNumPiMult<double, 2, 1> = 6.283185307179586;
// pi * 1/2 = 1.5707963267 9489661923 1321691639 7514420985 8469968755 ...
// bits (IEEE-754): float = +0x1.921fb6p+0f, double = +0x1.921fb54442d18p+0
template <> inline constexpr float cNumPiMult<float, 1, 2> = 1.57079633f;
template <>
inline constexpr double cNumPiMult<double, 1, 2> = 1.5707963267948966;
// pi * 3/2 = 4.7123889803 8468985769 3965074919 2543262957 5409906265 ...
// bits (IEEE-754): float = +0x1.2d97c8p+2f, double = +0x1.2d97c7f3321d2p+2
template <> inline constexpr float cNumPiMult<float, 3, 2> = 4.712389f;
template <> inline constexpr double cNumPiMult<double, 3, 2> = 4.71238898038469;
// pi * 1/3 = 1.0471975511 9659774615 4214461093 1676280657 2313312503 ...
// bits (IEEE-754): float = +0x1.0c1524p+0f, double = +0x1.0c152382d7366p+0
template <> inline constexpr float cNumPiMult<float, 1, 3> = 1.0471976f;
template <>
inline constexpr double cNumPiMult<double, 1, 3> = 1.04719755119659775;
// pi * 2/3 = 2.0943951023 9319549230 8428922186 3352561314 4626625007 ...
// bits (IEEE-754): float = +0x1.0c1524p+1f, double = +0x1.0c152382d7366p+1
template <> inline constexpr float cNumPiMult<float, 2, 3> = 2.0943951f;
template <>
inline constexpr double cNumPiMult<double, 2, 3> = 2.0943951023931955;
// pi * 4/3 = 4.1887902047 8639098461 6857844372 6705122628 9253250014 ...
// bits (IEEE-754): float = +0x1.0c1524p+2f, double = +0x1.0c152382d7366p+2
template <> inline constexpr float cNumPiMult<float, 4, 3> = 4.1887902f;
template <>
inline constexpr double cNumPiMult<double, 4, 3> = 4.188790204786391;
// pi * 5/3 = 5.2359877559 8298873077 1072305465 8381403286 1566562517 ...
// bits (IEEE-754): float = +0x1.4f1a6cp+2f, double = +0x1.4f1a6c638d03fp+2
template <> inline constexpr float cNumPiMult<float, 5, 3> = 5.2359878f;
template <>
inline constexpr double cNumPiMult<double, 5, 3> = 5.235987755982989;
// pi * 1/4 = 0.7853981633 9744830961 5660845819 8757210492 9234984377 ...
// bits (IEEE-754): float = +0x1.921fb6p-1f, double = +0x1.921fb54442d18p-1
template <> inline constexpr float cNumPiMult<float, 1, 4> = 0.7853982f;
template <>
inline constexpr double cNumPiMult<double, 1, 4> = 0.7853981633974483;
// pi * 3/4 = 2.3561944901 9234492884 6982537459 6271631478 7704953132 ...
// bits (IEEE-754): float = +0x1.2d97c8p+1f, double = +0x1.2d97c7f3321d2p+1
template <> inline constexpr float cNumPiMult<float, 3, 4> = 2.3561945f;
template <>
inline constexpr double cNumPiMult<double, 3, 4> = 2.356194490192345;
// pi * 5/4 = 3.9269908169 8724154807 8304229099 3786052464 6174921888 ...
// bits (IEEE-754): float = +0x1.f6a7a2p+1f, double = +0x1.f6a7a2955385ep+1
template <> inline constexpr float cNumPiMult<float, 5, 4> = 3.9269908f;
template <>
inline constexpr double cNumPiMult<double, 5, 4> = 3.9269908169872415;
// pi * 7/4 = 5.4977871437 8213816730 9625920739 1300473450 4644890643 ...
// bits (IEEE-754): float = +0x1.5fdbbep+2f, double = +0x1.5fdbbe9bba775p+2
template <> inline constexpr float cNumPiMult<float, 7, 4> = 5.497787f;
template <>
inline constexpr double cNumPiMult<double, 7, 4> = 5.497787143782138;
// pi * 1/6 = 0.5235987755 9829887307 7107230546 5838140328 6156656251 ...
// bits (IEEE-754): float = +0x1.0c1524p-1f, double = +0x1.0c152382d7366p-1
template <> inline constexpr float cNumPiMult<float, 1, 6> = 0.5235988f;
template <>
inline constexpr double cNumPiMult<double, 1, 6> = 0.523598775598298873;
// pi * 5/6 = 2.6179938779 9149436538 5536152732 9190701643 0783281258 ...
// bits (IEEE-754): float = +0x1.4f1a6cp+1f, double = +0x1.4f1a6c638d03fp+1
template <> inline constexpr float cNumPiMult<float, 5, 6> = 2.6179939f;
template <>
inline constexpr double cNumPiMult<double, 5, 6> = 2.6179938779914944;
// pi * 7/6 = 3.6651914291 8809211153 9750613826 0866982300 3096593762 ...
// bits (IEEE-754): float = +0x1.d524fep+1f, double = +0x1.d524fe24f89f2p+1
template <> inline constexpr float cNumPiMult<float, 7, 6> = 3.6651914f;
template <>
inline constexpr double cNumPiMult<double, 7, 6> = 3.6651914291880921;
// pi * 11/6 = 5.7595865315 8128760384 8179536012 4219543614 7723218769 ...
// bits (IEEE-754): float = +0x1.709d10p+2f, double = +0x1.709d10d3e7eacp+2
template <> inline constexpr float cNumPiMult<float, 11, 6> = 5.7595865f;
template <>
inline constexpr double cNumPiMult<double, 11, 6> = 5.759586531581288;

// === Rational multiples of inverse pi ===
// reciprocals of the pi multiples

/// Multiples of 1/pi (n/(d*pi)) hardcoded to closest IEEE values
template <cFpType T, int n, int d> inline constexpr T cNumInvPiMult;
// 1/(1*pi) = 0.3183098861 8379067153 7767526745 0287240689 1929148091 ...
// bits (IEEE-754): float = +0x1.45f306p-2f, double = +0x1.45f306dc9c883p-2
template <> inline constexpr float cNumInvPiMult<float, 1, 1> = 0.318309886f;
template <>
inline constexpr double cNumInvPiMult<double, 1, 1> = 0.3183098861837907;
// 1/(2*pi) = 0.1591549430 9189533576 8883763372 5143620344 5964574045 ...
// bits (IEEE-754): float = +0x1.45f306p-3f, double = +0x1.45f306dc9c883p-3
template <> inline constexpr float cNumInvPiMult<float, 1, 2> = 0.15915494f;
template <>
inline constexpr double cNumInvPiMult<double, 1, 2> = 0.15915494309189534;
// 2/(1*pi) = 0.6366197723 6758134307 5535053490 0574481378 3858296182 ...
// bits (IEEE-754): float = +0x1.45f306p-1f, double = +0x1.45f306dc9c883p-1
template <> inline constexpr float cNumInvPiMult<float, 2, 1> = 0.63661977f;
template <>
inline constexpr double cNumInvPiMult<double, 2, 1> = 0.63661977236758134;
// 2/(3*pi) = 0.2122065907 8919378102 5178351163 3524827126 1286098727 ...
// bits (IEEE-754): float = +0x1.b2995ep-3f, double = +0x1.b2995e7b7b604p-3
template <> inline constexpr float cNumInvPiMult<float, 2, 3> = 0.21220659f;
template <>
inline constexpr double cNumInvPiMult<double, 2, 3> = 0.212206590789193781;
// 3/(1*pi) = 0.9549296585 5137201461 3302580235 0861722067 5787444273 ...
// bits (IEEE-754): float = +0x1.e8ec8ap-1f, double = +0x1.e8ec8a4aeacc4p-1
template <> inline constexpr float cNumInvPiMult<float, 3, 1> = 0.95492966f;
template <>
inline constexpr double cNumInvPiMult<double, 3, 1> = 0.954929658551372;
// 3/(2*pi) = 0.4774648292 7568600730 6651290117 5430861033 7893722136 ...
// bits (IEEE-754): float = +0x1.e8ec8ap-2f, double = +0x1.e8ec8a4aeacc4p-2
template <> inline constexpr float cNumInvPiMult<float, 3, 2> = 0.47746483f;
template <>
inline constexpr double cNumInvPiMult<double, 3, 2> = 0.477464829275686;
// 3/(4*pi) = 0.2387324146 3784300365 3325645058 7715430516 8946861068 ...
// bits (IEEE-754): float = +0x1.e8ec8ap-3f, double = +0x1.e8ec8a4aeacc4p-3
template <> inline constexpr float cNumInvPiMult<float, 3, 4> = 0.23873241f;
template <>
inline constexpr double cNumInvPiMult<double, 3, 4> = 0.238732414637843;
// 3/(5*pi) = 0.1909859317 1027440292 2660516047 0172344413 5157488854 ...
// bits (IEEE-754): float = +0x1.8723a2p-3f, double = +0x1.8723a1d588a36p-3
template <> inline constexpr float cNumInvPiMult<float, 3, 5> = 0.19098593f;
template <>
inline constexpr double cNumInvPiMult<double, 3, 5> = 0.1909859317102744;
// 4/(1*pi) = 1.2732395447 3516268615 1070106980 1148962756 7716592365 ...
// bits (IEEE-754): float = +0x1.45f306p+0f, double = +0x1.45f306dc9c883p+0
template <> inline constexpr float cNumInvPiMult<float, 4, 1> = 1.2732395f;
template <>
inline constexpr double cNumInvPiMult<double, 4, 1> = 1.2732395447351627;
// 4/(3*pi) = 0.4244131815 7838756205 0356702326 7049654252 2572197455 ...
// bits (IEEE-754): float = +0x1.b2995ep-2f, double = +0x1.b2995e7b7b604p-2
template <> inline constexpr float cNumInvPiMult<float, 4, 3> = 0.42441318f;
template <>
inline constexpr double cNumInvPiMult<double, 4, 3> = 0.424413181578387562;
// 4/(5*pi) = 0.2546479089 4703253723 0214021396 0229792551 3543318473 ...
// bits (IEEE-754): float = +0x1.04c26cp-2f, double = +0x1.04c26be3b06cfp-2
template <> inline constexpr float cNumInvPiMult<float, 4, 5> = 0.2546479f;
template <>
inline constexpr double cNumInvPiMult<double, 4, 5> = 0.25464790894703254;
// 4/(7*pi) = 0.1818913635 3359466945 0152872425 7306994679 5388084623 ...
// bits (IEEE-754): float = +0x1.748376p-3f, double = +0x1.7483758e69c03p-3
template <> inline constexpr float cNumInvPiMult<float, 4, 7> = 0.18189136f;
template <>
inline constexpr double cNumInvPiMult<double, 4, 7> = 0.18189136353359467;
// 6/(1*pi) = 1.9098593171 0274402922 6605160470 1723444135 1574888547 ...
// bits (IEEE-754): float = +0x1.e8ec8ap+0f, double = +0x1.e8ec8a4aeacc4p+0
template <> inline constexpr float cNumInvPiMult<float, 6, 1> = 1.9098593f;
template <>
inline constexpr double cNumInvPiMult<double, 6, 1> = 1.909859317102744;
// 6/(5*pi) = 0.3819718634 2054880584 5321032094 0344688827 0314977709 ...
// bits (IEEE-754): float = +0x1.8723a2p-2f, double = +0x1.8723a1d588a36p-2
template <> inline constexpr float cNumInvPiMult<float, 6, 5> = 0.38197186f;
template <>
inline constexpr double cNumInvPiMult<double, 6, 5> = 0.3819718634205488;
// 6/(7*pi) = 0.2728370453 0039200417 5229308638 5960492019 3082126935 ...
// bits (IEEE-754): float = +0x1.176298p-2f, double = +0x1.1762982acf502p-2
template <> inline constexpr float cNumInvPiMult<float, 6, 7> = 0.27283705f;
template <>
inline constexpr double cNumInvPiMult<double, 6, 7> = 0.272837045300392;
// 6/(11*pi) = 0.1736235742 8206763902 0600469133 6520313103 1961353504 ...
// bits (IEEE-754): float = +0x1.6394c2p-3f, double = +0x1.6394c1aad94ecp-3
template <> inline constexpr float cNumInvPiMult<float, 6, 11> = 0.17362357f;
template <>
inline constexpr double cNumInvPiMult<double, 6, 11> = 0.17362357428206764;

// === Square roots of integers ===
// only below 25 for now

/// Square root of an integer. Currently non perfect squares below 25
template <cFpType T, int n> inline constexpr T cNumSqrt;
// sqrt(n) = 1.4142135623 7309504880 1688724209 6980785696 7187537694 ...
// bits (IEEE-754): float = +0x1.6a09e6p+0f, double = +0x1.6a09e667f3bcdp+0
template <> inline constexpr float cNumSqrt<float, 2> = 1.41421356f;
template <> inline constexpr double cNumSqrt<double, 2> = 1.41421356237309505;
// sqrt(n) = 1.7320508075 6887729352 7446341505 8723669428 0525381038 ...
// bits (IEEE-754): float = +0x1.bb67aep+0f, double = +0x1.bb67ae8584caap+0
template <> inline constexpr float cNumSqrt<float, 3> = 1.7320508f;
template <> inline constexpr double cNumSqrt<double, 3> = 1.7320508075688773;
// sqrt(n) = 2.2360679774 9978969640 9173668731 2762354406 1835961152 ...
// bits (IEEE-754): float = +0x1.1e377ap+1f, double = +0x1.1e3779b97f4a8p+1
template <> inline constexpr float cNumSqrt<float, 5> = 2.236068f;
template <> inline constexpr double cNumSqrt<double, 5> = 2.23606797749979;
// sqrt(n) = 2.4494897427 8317809819 7284074705 8913919659 4748065667 ...
// bits (IEEE-754): float = +0x1.3988e2p+1f, double = +0x1.3988e1409212ep+1
template <> inline constexpr float cNumSqrt<float, 6> = 2.44948974f;
template <> inline constexpr double cNumSqrt<double, 6> = 2.449489742783178;
// sqrt(n) = 2.6457513110 6459059050 1615753639 2604257102 5918308245 ...
// bits (IEEE-754): float = +0x1.52a7fap+1f, double = +0x1.52a7fa9d2f8eap+1
template <> inline constexpr float cNumSqrt<float, 7> = 2.6457513f;
template <> inline constexpr double cNumSqrt<double, 7> = 2.6457513110645906;
// sqrt(n) = 2.8284271247 4619009760 3377448419 3961571393 4375075389 ...
// bits (IEEE-754): float = +0x1.6a09e6p+1f, double = +0x1.6a09e667f3bcdp+1
template <> inline constexpr float cNumSqrt<float, 8> = 2.828427f;
template <> inline constexpr double cNumSqrt<double, 8> = 2.8284271247461901;
// sqrt(n) = 3.1622776601 6837933199 8893544432 7185337195 5513932521 ...
// bits (IEEE-754): float = +0x1.94c584p+1f, double = +0x1.94c583ada5b53p+1
template <> inline constexpr float cNumSqrt<float, 10> = 3.1622777f;
template <> inline constexpr double cNumSqrt<double, 10> = 3.16227766016837933;
// sqrt(n) = 3.3166247903 5539984911 4932736670 6866839270 8854558935 ...
// bits (IEEE-754): float = +0x1.a8872ap+1f, double = +0x1.a887293fd6f34p+1
template <> inline constexpr float cNumSqrt<float, 11> = 3.3166248f;
template <> inline constexpr double cNumSqrt<double, 11> = 3.3166247903554;
// sqrt(n) = 3.4641016151 3775458705 4892683011 7447338856 1050762076 ...
// bits (IEEE-754): float = +0x1.bb67aep+1f, double = +0x1.bb67ae8584caap+1
template <> inline constexpr float cNumSqrt<float, 12> = 3.4641016f;
template <> inline constexpr double cNumSqrt<double, 12> = 3.4641016151377546;
// sqrt(n) = 3.6055512754 6398929311 9221267470 4959462512 9657384524 ...
// bits (IEEE-754): float = +0x1.cd82b4p+1f, double = +0x1.cd82b446159f3p+1
template <> inline constexpr float cNumSqrt<float, 13> = 3.6055513f;
template <> inline constexpr double cNumSqrt<double, 13> = 3.605551275463989;
// sqrt(n) = 3.7416573867 7394138558 3748732316 5493017560 1980777872 ...
// bits (IEEE-754): float = +0x1.deeea2p+1f, double = +0x1.deeea11683f49p+1
template <> inline constexpr float cNumSqrt<float, 14> = 3.7416574f;
template <> inline constexpr double cNumSqrt<double, 14> = 3.7416573867739414;
// sqrt(n) = 3.8729833462 0741688517 9265399782 3996108329 2170529159 ...
// bits (IEEE-754): float = +0x1.efbdecp+1f, double = +0x1.efbdeb14f4edap+1
template <> inline constexpr float cNumSqrt<float, 15> = 3.87298335f;
template <> inline constexpr double cNumSqrt<double, 15> = 3.872983346207417;
// sqrt(n) = 4.1231056256 1766054982 1409855974 0770251471 9922537362 ...
// bits (IEEE-754): float = +0x1.07e0f6p+2f, double = +0x1.07e0f66afed07p+2
template <> inline constexpr float cNumSqrt<float, 17> = 4.1231056f;
template <> inline constexpr double cNumSqrt<double, 17> = 4.123105625617661;
// sqrt(n) = 4.2426406871 1928514640 5066172629 0942357090 1562613084 ...
// bits (IEEE-754): float = +0x1.0f876cp+2f, double = +0x1.0f876ccdf6cd9p+2
template <> inline constexpr float cNumSqrt<float, 18> = 4.2426407f;
template <> inline constexpr double cNumSqrt<double, 18> = 4.242640687119285;
// sqrt(n) = 4.3588989435 4067355223 6981983859 6156591370 0392523244 ...
// bits (IEEE-754): float = +0x1.16f834p+2f, double = +0x1.16f8334644df9p+2
template <> inline constexpr float cNumSqrt<float, 19> = 4.358899f;
template <> inline constexpr double cNumSqrt<double, 19> = 4.358898943540674;
// sqrt(n) = 4.4721359549 9957939281 8347337462 5524708812 3671922305 ...
// bits (IEEE-754): float = +0x1.1e377ap+2f, double = +0x1.1e3779b97f4a8p+2
template <> inline constexpr float cNumSqrt<float, 20> = 4.472136f;
template <> inline constexpr double cNumSqrt<double, 20> = 4.4721359549995794;
// sqrt(n) = 4.5825756949 5584000658 8047193728 0084889844 5657676797 ...
// bits (IEEE-754): float = +0x1.2548ecp+2f, double = +0x1.2548eb9151e85p+2
template <> inline constexpr float cNumSqrt<float, 21> = 4.582576f;
template <> inline constexpr double cNumSqrt<double, 21> = 4.58257569495584;
// sqrt(n) = 4.6904157598 2342955456 5630113544 4662805882 2835341173 ...
// bits (IEEE-754): float = +0x1.2c2fc6p+2f, double = +0x1.2c2fc595456a7p+2
template <> inline constexpr float cNumSqrt<float, 22> = 4.690416f;
template <> inline constexpr double cNumSqrt<double, 22> = 4.69041575982343;
// sqrt(n) = 4.7958315233 1271954159 7438064162 6939199967 0704190412 ...
// bits (IEEE-754): float = +0x1.32eee8p+2f, double = +0x1.32eee75770416p+2
template <> inline constexpr float cNumSqrt<float, 23> = 4.7958315f;
template <> inline constexpr double cNumSqrt<double, 23> = 4.7958315233127195;
// sqrt(n) = 4.8989794855 6635619639 4568149411 7827839318 9496131334 ...
// bits (IEEE-754): float = +0x1.3988e2p+2f, double = +0x1.3988e1409212ep+2
template <> inline constexpr float cNumSqrt<float, 24> = 4.8989795f;
template <> inline constexpr double cNumSqrt<double, 24> = 4.898979485566356;

// === Inverse square roots of integers ===
// only below 25 for now

/// Inverse square root of an integer. Currently non perfect squares below 25
template <cFpType T, int n> inline constexpr T cNumInvSqrt;
// 1/sqrt(n) = 0.7071067811 8654752440 0844362104 8490392848 3593768847 ...
// bits (IEEE-754): float = +0x1.6a09e6p-1f, double = +0x1.6a09e667f3bcdp-1
template <> inline constexpr float cNumInvSqrt<float, 2> = 0.70710678f;
template <>
inline constexpr double cNumInvSqrt<double, 2> = 0.70710678118654752;
// 1/sqrt(n) = 0.5773502691 8962576450 9148780501 9574556476 0175127012 ...
// bits (IEEE-754): float = +0x1.279a74p-1f, double = +0x1.279a74590331cp-1
template <> inline constexpr float cNumInvSqrt<float, 3> = 0.57735027f;
template <>
inline constexpr double cNumInvSqrt<double, 3> = 0.57735026918962576;
// 1/sqrt(n) = 0.4472135954 9995793928 1834733746 2552470881 2367192230 ...
// bits (IEEE-754): float = +0x1.c9f25cp-2f, double = +0x1.c9f25c5bfedd9p-2
template <> inline constexpr float cNumInvSqrt<float, 5> = 0.4472136f;
template <> inline constexpr double cNumInvSqrt<double, 5> = 0.4472135954999579;
// 1/sqrt(n) = 0.4082482904 6386301636 6214012450 9818986609 9124677611 ...
// bits (IEEE-754): float = +0x1.a20bd8p-2f, double = +0x1.a20bd700c2c3ep-2
template <> inline constexpr float cNumInvSqrt<float, 6> = 0.4082482905f;
template <> inline constexpr double cNumInvSqrt<double, 6> = 0.408248290463863;
// 1/sqrt(n) = 0.3779644730 0922722721 4516536234 1800608157 5131186892 ...
// bits (IEEE-754): float = +0x1.83091ep-2f, double = +0x1.83091e6a7f7e7p-2
template <> inline constexpr float cNumInvSqrt<float, 7> = 0.37796447f;
template <>
inline constexpr double cNumInvSqrt<double, 7> = 0.37796447300922723;
// 1/sqrt(n) = 0.3535533905 9327376220 0422181052 4245196424 1796884423 ...
// bits (IEEE-754): float = +0x1.6a09e6p-2f, double = +0x1.6a09e667f3bcdp-2
template <> inline constexpr float cNumInvSqrt<float, 8> = 0.35355339f;
template <> inline constexpr double cNumInvSqrt<double, 8> = 0.3535533905932738;
// 1/sqrt(n) = 0.3162277660 1683793319 9889354443 2718533719 5551393252 ...
// bits (IEEE-754): float = +0x1.43d136p-2f, double = +0x1.43d136248490fp-2
template <> inline constexpr float cNumInvSqrt<float, 10> = 0.31622777f;
template <>
inline constexpr double cNumInvSqrt<double, 10> = 0.31622776601683793;
// 1/sqrt(n) = 0.3015113445 7776362264 6812066970 0624258115 5350414448 ...
// bits (IEEE-754): float = +0x1.34bf64p-2f, double = +0x1.34bf63d156826p-2
template <> inline constexpr float cNumInvSqrt<float, 11> = 0.30151134f;
template <>
inline constexpr double cNumInvSqrt<double, 11> = 0.30151134457776362;
// 1/sqrt(n) = 0.2886751345 9481288225 4574390250 9787278238 0087563506 ...
// bits (IEEE-754): float = +0x1.279a74p-2f, double = +0x1.279a74590331cp-2
template <> inline constexpr float cNumInvSqrt<float, 12> = 0.28867513f;
template <>
inline constexpr double cNumInvSqrt<double, 12> = 0.28867513459481288;
// 1/sqrt(n) = 0.2773500981 1261456100 9170866728 4996881731 7665952655 ...
// bits (IEEE-754): float = +0x1.1c01aap-2f, double = +0x1.1c01aa03be896p-2
template <> inline constexpr float cNumInvSqrt<float, 13> = 0.2773501f;
template <>
inline constexpr double cNumInvSqrt<double, 13> = 0.2773500981126146;
// 1/sqrt(n) = 0.2672612419 1242438468 4553480879 7535215540 0141484133 ...
// bits (IEEE-754): float = +0x1.11aceep-2f, double = +0x1.11acee560242ap-2
template <> inline constexpr float cNumInvSqrt<float, 14> = 0.26726124f;
template <>
inline constexpr double cNumInvSqrt<double, 14> = 0.2672612419124244;
// 1/sqrt(n) = 0.2581988897 4716112567 8617693318 8266407221 9478035277 ...
// bits (IEEE-754): float = +0x1.08654ap-2f, double = +0x1.08654a2d4f6dbp-2
template <> inline constexpr float cNumInvSqrt<float, 15> = 0.2581989f;
template <>
inline constexpr double cNumInvSqrt<double, 15> = 0.25819888974716113;
// 1/sqrt(n) = 0.2425356250 3633297351 8906462116 1221779498 3524855138 ...
// bits (IEEE-754): float = +0x1.f0b684p-3f, double = +0x1.f0b6848d2af1cp-3
template <> inline constexpr float cNumInvSqrt<float, 17> = 0.242535625f;
template <>
inline constexpr double cNumInvSqrt<double, 17> = 0.24253562503633297;
// 1/sqrt(n) = 0.2357022603 9551584146 6948120701 6163464282 7864589615 ...
// bits (IEEE-754): float = +0x1.e2b7dep-3f, double = +0x1.e2b7dddfefa66p-3
template <> inline constexpr float cNumInvSqrt<float, 18> = 0.23570226f;
template <>
inline constexpr double cNumInvSqrt<double, 18> = 0.23570226039551584;
// 1/sqrt(n) = 0.2294157338 7056176590 7209578097 8745083756 3178553854 ...
// bits (IEEE-754): float = +0x1.d5d7eap-3f, double = +0x1.d5d7ea914b937p-3
template <> inline constexpr float cNumInvSqrt<float, 19> = 0.22941573f;
template <>
inline constexpr double cNumInvSqrt<double, 19> = 0.22941573387056177;
// 1/sqrt(n) = 0.2236067977 4997896964 0917366873 1276235440 6183596115 ...
// bits (IEEE-754): float = +0x1.c9f25cp-3f, double = +0x1.c9f25c5bfedd9p-3
template <> inline constexpr float cNumInvSqrt<float, 20> = 0.2236068f;
template <>
inline constexpr double cNumInvSqrt<double, 20> = 0.22360679774997897;
// 1/sqrt(n) = 0.2182178902 3599238126 6097485415 6194518564 0269413180 ...
// bits (IEEE-754): float = +0x1.bee906p-3f, double = +0x1.bee9056fb9c39p-3
template <> inline constexpr float cNumInvSqrt<float, 21> = 0.2182179f;
template <>
inline constexpr double cNumInvSqrt<double, 21> = 0.2182178902359924;
// 1/sqrt(n) = 0.2132007163 5561043429 8437732433 8393763903 7401606416 ...
// bits (IEEE-754): float = +0x1.b4a294p-3f, double = +0x1.b4a293c1d9550p-3
template <> inline constexpr float cNumInvSqrt<float, 22> = 0.21320072f;
template <>
inline constexpr double cNumInvSqrt<double, 22> = 0.21320071635561043;
// 1/sqrt(n) = 0.2085144140 5707476267 8149481050 5519095650 7421921322 ...
// bits (IEEE-754): float = +0x1.ab099ap-3f, double = +0x1.ab099ae8f539ap-3
template <> inline constexpr float cNumInvSqrt<float, 23> = 0.2085144f;
template <>
inline constexpr double cNumInvSqrt<double, 23> = 0.20851441405707476;
// 1/sqrt(n) = 0.2041241452 3193150818 3107006225 4909493304 9562338805 ...
// bits (IEEE-754): float = +0x1.a20bd8p-3f, double = +0x1.a20bd700c2c3ep-3
template <> inline constexpr float cNumInvSqrt<float, 24> = 0.20412414523f;
template <>
inline constexpr double cNumInvSqrt<double, 24> = 0.2041241452319315;

// === Cube roots of integers ===
// only below 27 for now

/// Cube root of an integer. Currently non perfect cubes below 25
template <cFpType T, int n> inline constexpr T cNumCbrt;
// cbrt(n) = 1.2599210498 9487316476 7210607278 2283505702 5146470150 ...
// bits (IEEE-754): float = +0x1.428a30p+0f, double = +0x1.428a2f98d728bp+0
template <> inline constexpr float cNumCbrt<float, 2> = 1.25992105f;
template <> inline constexpr double cNumCbrt<double, 2> = 1.2599210498948732;
// cbrt(n) = 1.4422495703 0740838232 1638310780 1095883918 6925349935 ...
// bits (IEEE-754): float = +0x1.713744p+0f, double = +0x1.7137449123ef6p+0
template <> inline constexpr float cNumCbrt<float, 3> = 1.44224957f;
template <> inline constexpr double cNumCbrt<double, 3> = 1.4422495703074084;
// cbrt(n) = 1.5874010519 6819947475 1705639272 3082603914 9332789985 ...
// bits (IEEE-754): float = +0x1.965feap+0f, double = +0x1.965fea53d6e3dp+0
template <> inline constexpr float cNumCbrt<float, 4> = 1.58740105f;
template <> inline constexpr double cNumCbrt<double, 4> = 1.587401051968199475;
// cbrt(n) = 1.7099759466 7669698935 3108872543 8601098680 5511054305 ...
// bits (IEEE-754): float = +0x1.b5c0fcp+0f, double = +0x1.b5c0fbcfec4d4p+0
template <> inline constexpr float cNumCbrt<float, 5> = 1.709976f;
template <> inline constexpr double cNumCbrt<double, 5> = 1.709975946676697;
// cbrt(n) = 1.8171205928 3213965889 1211756327 2605024282 1046314121 ...
// bits (IEEE-754): float = +0x1.d12ed0p+0f, double = +0x1.d12ed0af1a27fp+0
template <> inline constexpr float cNumCbrt<float, 6> = 1.8171206f;
template <> inline constexpr double cNumCbrt<double, 6> = 1.8171205928321397;
// cbrt(n) = 1.9129311827 7238910119 9116839548 7602828624 3905034587 ...
// bits (IEEE-754): float = +0x1.e9b5dcp+0f, double = +0x1.e9b5dba58189ep+0
template <> inline constexpr float cNumCbrt<float, 7> = 1.9129312f;
template <> inline constexpr double cNumCbrt<double, 7> = 1.9129311827723891;
// cbrt(n) = 2.0800838230 5190411453 0056824357 8853863378 0534037326 ...
// bits (IEEE-754): float = +0x1.0a4030p+1f, double = +0x1.0a402fcc79298p+1
template <> inline constexpr float cNumCbrt<float, 9> = 2.0800838f;
template <> inline constexpr double cNumCbrt<double, 9> = 2.080083823051904;
// cbrt(n) = 2.1544346900 3188372175 9293566519 3504952593 4494219210 ...
// bits (IEEE-754): float = +0x1.13c484p+1f, double = +0x1.13c484138704fp+1
template <> inline constexpr float cNumCbrt<float, 10> = 2.1544347f;
template <> inline constexpr double cNumCbrt<double, 10> = 2.154434690031884;
// cbrt(n) = 2.2239800905 6931552116 5363376722 1571965186 9912809692 ...
// bits (IEEE-754): float = +0x1.1cab62p+1f, double = +0x1.1cab612df9a46p+1
template <> inline constexpr float cNumCbrt<float, 11> = 2.2239801f;
template <> inline constexpr double cNumCbrt<double, 11> = 2.2239800905693155;
// cbrt(n) = 2.2894284851 0666373561 6084423879 3540178318 1384157586 ...
// bits (IEEE-754): float = +0x1.250bfep+1f, double = +0x1.250bfe1b082f5p+1
template <> inline constexpr float cNumCbrt<float, 12> = 2.2894285f;
template <> inline constexpr double cNumCbrt<double, 12> = 2.2894284851066637;
// cbrt(n) = 2.3513346877 2075748950 0016339956 9145269158 4198346217 ...
// bits (IEEE-754): float = +0x1.2cf888p+1f, double = +0x1.2cf888f8db02fp+1
template <> inline constexpr float cNumCbrt<float, 13> = 2.35133469f;
template <> inline constexpr double cNumCbrt<double, 13> = 2.3513346877207575;
// cbrt(n) = 2.4101422641 7522998612 8369667603 2728953545 8128998086 ...
// bits (IEEE-754): float = +0x1.347f8ap+1f, double = +0x1.347f8aadab855p+1
template <> inline constexpr float cNumCbrt<float, 14> = 2.4101423f;
template <> inline constexpr double cNumCbrt<double, 14> = 2.41014226417523;
// cbrt(n) = 2.4662120743 3047010149 1611323154 5890427354 8448662805 ...
// bits (IEEE-754): float = +0x1.3bacd6p+1f, double = +0x1.3bacd6561ff5ep+1
template <> inline constexpr float cNumCbrt<float, 15> = 2.466212f;
template <> inline constexpr double cNumCbrt<double, 15> = 2.4662120743304701;
// cbrt(n) = 2.5198420997 8974632953 4421214556 4567011405 0292940301 ...
// bits (IEEE-754): float = +0x1.428a30p+1f, double = +0x1.428a2f98d728bp+1
template <> inline constexpr float cNumCbrt<float, 16> = 2.5198421f;
template <> inline constexpr double cNumCbrt<double, 16> = 2.5198420997897463;
// cbrt(n) = 2.5712815906 5823535545 3187208739 7261164279 0163245469 ...
// bits (IEEE-754): float = +0x1.491fc2p+1f, double = +0x1.491fc152578cap+1
template <> inline constexpr float cNumCbrt<float, 17> = 2.5712816f;
template <> inline constexpr double cNumCbrt<double, 17> = 2.57128159065823536;
// cbrt(n) = 2.6207413942 0889660714 1661280441 9962702394 2764572363 ...
// bits (IEEE-754): float = +0x1.4f7474p+1f, double = +0x1.4f747439b348ap+1
template <> inline constexpr float cNumCbrt<float, 18> = 2.6207414f;
template <> inline constexpr double cNumCbrt<double, 18> = 2.6207413942088966;
// cbrt(n) = 2.6684016487 2194486733 9627371970 8303350958 7856918310 ...
// bits (IEEE-754): float = +0x1.558e30p+1f, double = +0x1.558e2f6aed36cp+1
template <> inline constexpr float cNumCbrt<float, 19> = 2.6684016f;
template <> inline constexpr double cNumCbrt<double, 19> = 2.668401648721945;
// cbrt(n) = 2.7144176165 9490657151 8089469679 4892048051 0776948909 ...
// bits (IEEE-754): float = +0x1.5b720ap+1f, double = +0x1.5b7209557b0eep+1
template <> inline constexpr float cNumCbrt<float, 20> = 2.7144176f;
template <> inline constexpr double cNumCbrt<double, 20> = 2.7144176165949066;
// cbrt(n) = 2.7589241763 8112066946 5791108358 5215822527 1208603893 ...
// bits (IEEE-754): float = +0x1.61246ep+1f, double = +0x1.61246d6ad9aeep+1
template <> inline constexpr float cNumCbrt<float, 21> = 2.7589242f;
template <> inline constexpr double cNumCbrt<double, 21> = 2.7589241763811207;
// cbrt(n) = 2.8020393306 5538712066 5677385665 8940175857 9821876926 ...
// bits (IEEE-754): float = +0x1.66a93ap+1f, double = +0x1.66a9398ba2a3ap+1
template <> inline constexpr float cNumCbrt<float, 22> = 2.8020393f;
template <> inline constexpr double cNumCbrt<double, 22> = 2.8020393306553871;
// cbrt(n) = 2.8438669798 5156547769 5439400958 6518527641 6517273704 ...
// bits (IEEE-754): float = +0x1.6c03d6p+1f, double = +0x1.6c03d54c51818p+1
template <> inline constexpr float cNumCbrt<float, 23> = 2.843867f;
template <> inline constexpr double cNumCbrt<double, 23> = 2.8438669798515655;
// cbrt(n) = 2.8844991406 1481676464 3276621560 2191767837 3850699870 ...
// bits (IEEE-754): float = +0x1.713744p+1f, double = +0x1.7137449123ef6p+1
template <> inline constexpr float cNumCbrt<float, 24> = 2.884499f;
template <> inline constexpr double cNumCbrt<double, 24> = 2.8844991406148168;
// cbrt(n) = 2.9240177382 1286606550 6787360137 9227785304 9863510103 ...
// bits (IEEE-754): float = +0x1.764636p+1f, double = +0x1.764636974629cp+1
template <> inline constexpr float cNumCbrt<float, 25> = 2.9240177f;
template <> inline constexpr double cNumCbrt<double, 25> = 2.924017738212866;
// cbrt(n) = 2.9624960684 0737050867 3062189341 8385375663 5742231886 ...
// bits (IEEE-754): float = +0x1.7b3312p+1f, double = +0x1.7b3312382b4b5p+1
template <> inline constexpr float cNumCbrt<float, 26> = 2.962496f;
template <> inline constexpr double cNumCbrt<double, 26> = 2.9624960684073705;

// === Inverse cube roots of integers ===
// only below 27 for now

/// Inverse cube root of an integer. Currently non perfect cubes below 25
template <cFpType T, int n> inline constexpr T cNumInvCbrt;
// 1/cbrt(n) = 0.7937005259 8409973737 5852819636 1541301957 4666394992 ...
// bits (IEEE-754): float = +0x1.965feap-1f, double = +0x1.965fea53d6e3dp-1
template <> inline constexpr float cNumInvCbrt<float, 2> = 0.7937005f;
template <>
inline constexpr double cNumInvCbrt<double, 2> = 0.79370052598409974;
// 1/cbrt(n) = 0.6933612743 5063470484 3352274785 9617954459 3511345775 ...
// bits (IEEE-754): float = +0x1.630040p-1f, double = +0x1.63003fbb4c375p-1
template <> inline constexpr float cNumInvCbrt<float, 3> = 0.6933613f;
template <> inline constexpr double cNumInvCbrt<double, 3> = 0.6933612743506347;
// 1/cbrt(n) = 0.6299605249 4743658238 3605303639 1141752851 2573235075 ...
// bits (IEEE-754): float = +0x1.428a30p-1f, double = +0x1.428a2f98d728bp-1
template <> inline constexpr float cNumInvCbrt<float, 4> = 0.62996052f;
template <> inline constexpr double cNumInvCbrt<double, 4> = 0.6299605249474366;
// 1/cbrt(n) = 0.5848035476 4257321310 1357472027 5845557060 9972702020 ...
// bits (IEEE-754): float = +0x1.2b6b5ep-1f, double = +0x1.2b6b5edf6b54ap-1
template <> inline constexpr float cNumInvCbrt<float, 5> = 0.5848035f;
template <> inline constexpr double cNumInvCbrt<double, 5> = 0.5848035476425732;
// 1/cbrt(n) = 0.5503212081 4910444731 2434992068 1808261411 4744071974 ...
// bits (IEEE-754): float = +0x1.19c3b4p-1f, double = +0x1.19c3b38e975a8p-1
template <> inline constexpr float cNumInvCbrt<float, 6> = 0.5503212f;
template <>
inline constexpr double cNumInvCbrt<double, 6> = 0.55032120814910445;
// 1/cbrt(n) = 0.5227579585 7471021674 8296187159 9154662124 4338126333 ...
// bits (IEEE-754): float = +0x1.0ba6eep-1f, double = +0x1.0ba6ee5f9aad4p-1
template <> inline constexpr float cNumInvCbrt<float, 7> = 0.52275796f;
template <> inline constexpr double cNumInvCbrt<double, 7> = 0.5227579585747102;
// 1/cbrt(n) = 0.4807498567 6913612744 0546103593 3698627972 8975116645 ...
// bits (IEEE-754): float = +0x1.ec49b0p-2f, double = +0x1.ec49b0c1853f3p-2
template <> inline constexpr float cNumInvCbrt<float, 9> = 0.48074986f;
template <> inline constexpr double cNumInvCbrt<double, 9> = 0.4807498567691361;
// 1/cbrt(n) = 0.4641588833 6127788924 1007635091 9446576551 3491250112 ...
// bits (IEEE-754): float = +0x1.db4c78p-2f, double = +0x1.db4c7760bcff2p-2
template <> inline constexpr float cNumInvCbrt<float, 10> = 0.4641589f;
template <>
inline constexpr double cNumInvCbrt<double, 10> = 0.46415888336127789;
// 1/cbrt(n) = 0.4496443130 2260916984 7600332502 7530407989 7652606333 ...
// bits (IEEE-754): float = +0x1.cc6f90p-2f, double = +0x1.cc6f8f0d0ed75p-2
template <> inline constexpr float cNumInvCbrt<float, 11> = 0.449644313f;
template <>
inline constexpr double cNumInvCbrt<double, 11> = 0.44964431302260917;
// 1/cbrt(n) = 0.4367902323 6814943452 3610213406 9993783732 3794095393 ...
// bits (IEEE-754): float = +0x1.bf45f0p-2f, double = +0x1.bf45f04cef0b9p-2
template <> inline constexpr float cNumInvCbrt<float, 12> = 0.43679023f;
template <>
inline constexpr double cNumInvCbrt<double, 12> = 0.43679023236814943;
// 1/cbrt(n) = 0.4252903702 8299016472 8641905621 9477376773 4440588383 ...
// bits (IEEE-754): float = +0x1.b37f52p-2f, double = +0x1.b37f519ead3bbp-2
template <> inline constexpr float cNumInvCbrt<float, 13> = 0.42529037f;
template <>
inline constexpr double cNumInvCbrt<double, 13> = 0.42529037028299016;
// 1/cbrt(n) = 0.4149132666 8312172050 0495748475 0791289835 5874025628 ...
// bits (IEEE-754): float = +0x1.a8df06p-2f, double = +0x1.a8df05fc52779p-2
template <> inline constexpr float cNumInvCbrt<float, 14> = 0.41491327f;
template <>
inline constexpr double cNumInvCbrt<double, 14> = 0.41491326668312172;
// 1/cbrt(n) = 0.4054801330 3822667899 3229666515 8534946585 1138070066 ...
// bits (IEEE-754): float = +0x1.9f3630p-2f, double = +0x1.9f362f1a4ec23p-2
template <> inline constexpr float cNumInvCbrt<float, 15> = 0.405480133f;
template <>
inline constexpr double cNumInvCbrt<double, 15> = 0.40548013303822668;
// 1/cbrt(n) = 0.3968502629 9204986868 7926409818 0770650978 7333197496 ...
// bits (IEEE-754): float = +0x1.965feap-2f, double = +0x1.965fea53d6e3dp-2
template <> inline constexpr float cNumInvCbrt<float, 16> = 0.39685026f;
template <>
inline constexpr double cNumInvCbrt<double, 16> = 0.3968502629920499;
// 1/cbrt(n) = 0.3889111187 3282029430 3590972734 1454965461 4142068165 ...
// bits (IEEE-754): float = +0x1.8e3eb8p-2f, double = +0x1.8e3eb760086f1p-2
template <> inline constexpr float cNumInvCbrt<float, 17> = 0.38891112f;
template <>
inline constexpr double cNumInvCbrt<double, 17> = 0.3889111187328203;
// 1/cbrt(n) = 0.3815714141 8444395593 6014070646 5590029719 6897359597 ...
// bits (IEEE-754): float = +0x1.86baa8p-2f, double = +0x1.86baa8240ae9cp-2
template <> inline constexpr float cNumInvCbrt<float, 18> = 0.3815714f;
template <>
inline constexpr double cNumInvCbrt<double, 18> = 0.38157141418444396;
// 1/cbrt(n) = 0.3747561767 8431545537 2084763243 3567112150 4722337498 ...
// bits (IEEE-754): float = +0x1.7fc016p-2f, double = +0x1.7fc0154d0ceebp-2
template <> inline constexpr float cNumInvCbrt<float, 19> = 0.3747562f;
template <>
inline constexpr double cNumInvCbrt<double, 19> = 0.37475617678431546;
// 1/cbrt(n) = 0.3684031498 6403866057 7982283357 9807221917 2581174381 ...
// bits (IEEE-754): float = +0x1.793eacp-2f, double = +0x1.793eace1a3426p-2
template <> inline constexpr float cNumInvCbrt<float, 20> = 0.36840315f;
template <>
inline constexpr double cNumInvCbrt<double, 20> = 0.36840314986403866;
// 1/cbrt(n) = 0.3624601243 3429738257 4771673920 3035493652 4874584116 ...
// bits (IEEE-754): float = +0x1.7328c0p-2f, double = +0x1.7328bf307ac75p-2
template <> inline constexpr float cNumInvCbrt<float, 21> = 0.362460124f;
template <>
inline constexpr double cNumInvCbrt<double, 21> = 0.36246012433429738;
// 1/cbrt(n) = 0.3568829277 5180408533 5949995881 3306237953 9827379451 ...
// bits (IEEE-754): float = +0x1.6d72b8p-2f, double = +0x1.6d72b7dcc7672p-2
template <> inline constexpr float cNumInvCbrt<float, 22> = 0.35688293f;
template <>
inline constexpr double cNumInvCbrt<double, 22> = 0.3568829277518041;
// 1/cbrt(n) = 0.3516338869 1695931855 1043740233 3201650762 5615754153 ...
// bits (IEEE-754): float = +0x1.6812b6p-2f, double = +0x1.6812b6b1e5124p-2
template <> inline constexpr float cNumInvCbrt<float, 23> = 0.35163389f;
template <>
inline constexpr double cNumInvCbrt<double, 23> = 0.3516338869169593;
// 1/cbrt(n) = 0.3466806371 7531735242 1676137392 9808977229 6755672887 ...
// bits (IEEE-754): float = +0x1.630040p-2f, double = +0x1.63003fbb4c375p-2
template <> inline constexpr float cNumInvCbrt<float, 24> = 0.34668064f;
template <>
inline constexpr double cNumInvCbrt<double, 24> = 0.34668063717531735;
// 1/cbrt(n) = 0.3419951893 3533939787 0621774508 7720219736 1102210861 ...
// bits (IEEE-754): float = +0x1.5e33fcp-2f, double = +0x1.5e33fca656a43p-2
template <> inline constexpr float cNumInvCbrt<float, 25> = 0.34199519f;
template <>
inline constexpr double cNumInvCbrt<double, 25> = 0.3419951893353394;
// 1/cbrt(n) = 0.3375531905 8958183401 8929384547 1494533573 4518913580 ...
// bits (IEEE-754): float = +0x1.59a78cp-2f, double = +0x1.59a78b28f888bp-2
template <> inline constexpr float cNumInvCbrt<float, 26> = 0.3375532f;
template <>
inline constexpr double cNumInvCbrt<double, 26> = 0.33755319058958183;

} // namespace tkoz::ff::fpMath
