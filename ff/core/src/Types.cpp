#include <tkoz/ff/Types.hpp>

static_assert(sizeof(tkoz::ff::NumberValue32) == 4);
static_assert(sizeof(tkoz::ff::HistogramValue32) == 4);

static_assert(sizeof(tkoz::ff::NumberValue64) == 8);
static_assert(sizeof(tkoz::ff::HistogramValue64) == 8);
