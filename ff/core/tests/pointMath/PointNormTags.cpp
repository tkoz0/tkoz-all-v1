#include <tkoz/ff/pointMath/PointNormTags.hpp>

using namespace tkoz::ff;

class Dummy {};

template <auto P>
concept cValidPNormCt = requires { typename PNormCt<P>; };

template <typename T>
concept cValidPNormRt = requires { typename PNormRt<T>; };

// Test that compile time p norms are invalid for p <= 0 or bool
static_assert(!cValidPNormCt<0>);
static_assert(!cValidPNormCt<0u>);
static_assert(!cValidPNormCt<-1>);
static_assert(!cValidPNormCt<-2l>);
static_assert(!cValidPNormCt<-4ll>);
static_assert(!cValidPNormCt<0.99>);
static_assert(!cValidPNormCt<0.9>);
static_assert(!cValidPNormCt<0.0f>);
static_assert(!cValidPNormCt<-1.0>);
static_assert(!cValidPNormCt<-2.0f>);
static_assert(!cValidPNormCt<false>);
static_assert(!cValidPNormCt<true>);
static_assert(!cValidPNormCt<Dummy{}>);

// Test some valid compile time p norms
static_assert(cValidPNormCt<1>);
static_assert(cValidPNormCt<2u>);
static_assert(cValidPNormCt<3l>);
static_assert(cValidPNormCt<4ul>);
static_assert(cValidPNormCt<5ll>);
static_assert(cValidPNormCt<6ull>);
static_assert(cValidPNormCt<1.0>);
static_assert(cValidPNormCt<1.1f>);
static_assert(cValidPNormCt<1.2>);
static_assert(cValidPNormCt<1.5>);
static_assert(cValidPNormCt<2.0>);
static_assert(cValidPNormCt<2.7f>);

// Check for types with runtime p norm
static_assert(!cValidPNormRt<bool>);
static_assert(cValidPNormRt<char>);
static_assert(cValidPNormRt<signed char>);
static_assert(cValidPNormRt<unsigned char>);
static_assert(cValidPNormRt<signed short>);
static_assert(cValidPNormRt<unsigned short>);
static_assert(cValidPNormRt<signed int>);
static_assert(cValidPNormRt<unsigned int>);
static_assert(cValidPNormRt<signed long>);
static_assert(cValidPNormRt<unsigned long>);
static_assert(cValidPNormRt<signed long long>);
static_assert(cValidPNormRt<unsigned long long>);
static_assert(cValidPNormRt<float>);
static_assert(cValidPNormRt<double>);
static_assert(cValidPNormRt<long double>);
static_assert(!cValidPNormRt<Dummy>);
