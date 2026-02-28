#include <tkoz/ff/Types.hpp>

static_assert(tkoz::ff::cPrimitiveFpType<float>);
static_assert(tkoz::ff::cPrimitiveFpType<double>);
static_assert(!tkoz::ff::cPrimitiveFpType<long double>);
static_assert(!tkoz::ff::cPrimitiveFpType<int>);

static_assert(tkoz::ff::cNumberType<tkoz::ff::Number<float>>);
static_assert(tkoz::ff::cNumberType<tkoz::ff::Number<double>>);
static_assert(!tkoz::ff::cNumberType<float>);
static_assert(!tkoz::ff::cNumberType<double>);
