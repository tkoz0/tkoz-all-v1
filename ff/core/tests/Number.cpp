#include <tkoz/ff/Number.hpp>

#include <tkoz/SRTest.hpp>

#include <compare>
#include <concepts>
#include <utility>

using Fp32 = tkoz::ff::NumberFp32;
using Fp64 = tkoz::ff::NumberFp64;

// Sizes should match for these thin wrappers around float/double
static_assert(sizeof(Fp32) == sizeof(float));
static_assert(sizeof(Fp64) == sizeof(double));

// Implicit conversions allowed up to our wrapper type but not back
// For safety, we disallow mixing float and double
static_assert(std::assignable_from<Fp32 &, float>);
static_assert(!std::assignable_from<Fp32 &, double>);
static_assert(!std::assignable_from<Fp64 &, float>);
static_assert(std::assignable_from<Fp64 &, double>);
static_assert(!std::assignable_from<float &, Fp32>);
static_assert(!std::assignable_from<float &, Fp64>);
static_assert(!std::assignable_from<double &, Fp32>);
static_assert(!std::assignable_from<double &, Fp64>);

// This also tests for implicit convertibility
static_assert(std::convertible_to<float, Fp32>);
static_assert(!std::convertible_to<float, Fp64>);
static_assert(!std::convertible_to<double, Fp32>);
static_assert(std::convertible_to<double, Fp64>);
static_assert(!std::convertible_to<Fp32, float>);
static_assert(!std::convertible_to<Fp32, double>);
static_assert(!std::convertible_to<Fp64, float>);
static_assert(!std::convertible_to<Fp64, double>);

TEST_CREATE_FAST(ctorDefault) {
  // Default does not initialize memory
  float a = -1.561e20f;
  double b = 3.6718e11;
  Fp32 *ap = new (&a) Fp32;
  Fp64 *bp = new (&b) Fp64;
  TEST_REQUIRE_EQ(a, -1.561e20f);
  TEST_REQUIRE_EQ(b, 3.6718e11);
  TEST_REQUIRE_EQ(*ap, -1.561e20f);
  TEST_REQUIRE_EQ(*bp, 3.6718e11);
}

TEST_CREATE_FAST(ctorValue) {
  Fp32 a(7.6542f);
  Fp64 b(-1.2346);
  TEST_REQUIRE_EQ(7.6542f, a);
  TEST_REQUIRE_EQ(-1.2346, b);
}

TEST_CREATE_FAST(assignPrimitive) {
  Fp32 a;
  Fp64 b;
  a = 7.6541f;
  b = -1.2347;
  TEST_REQUIRE_EQ(7.6541f, a);
  TEST_REQUIRE_EQ(-1.2347, b);
}

TEST_CREATE_FAST(conv) {
  Fp32 a(-3.45f);
  Fp64 b(8.7654);
  TEST_REQUIRE_EQ(-3.45f, float{a});
  TEST_REQUIRE_EQ(8.7654, double{b});
  TEST_REQUIRE_EQ(-3.45f, static_cast<float>(a));
  TEST_REQUIRE_EQ(8.7654, static_cast<double>(b));
}

TEST_CREATE_FAST(ruleOf5) {
  {
    // Copy
    Fp32 a = 5.81f;
    Fp32 b(a);
    TEST_REQUIRE_EQ(a, b);
    TEST_REQUIRE_EQ(a, 5.81f);
    TEST_REQUIRE_EQ(b, 5.81f);
    Fp32 c = a;
    TEST_REQUIRE_EQ(a, c);
    TEST_REQUIRE_EQ(c, 5.81f);
  }
  {
    // Move (should do exactly the same as copy)
    Fp64 a = 377.43;
    Fp64 b = 377.43;
    TEST_REQUIRE_EQ(a, b);
    Fp64 c(std::move(a));
    Fp64 d = std::move(b);
    TEST_REQUIRE_EQ(a, c);
    TEST_REQUIRE_EQ(b, d);
    TEST_REQUIRE_EQ(c, 377.43);
    TEST_REQUIRE_EQ(d, 377.43);
  }
}

TEST_CREATE_FAST(dtor) {
  // Should do nothing
  float a;
  double b;
  Fp32 *ap = new (&a) Fp32(-1.561e20f);
  Fp64 *bp = new (&b) Fp64(3.6718e11);
  TEST_REQUIRE_EQ(a, -1.561e20f);
  TEST_REQUIRE_EQ(b, 3.6718e11);
  TEST_REQUIRE_EQ(*ap, -1.561e20f);
  TEST_REQUIRE_EQ(*bp, 3.6718e11);
  ap->~Fp32();
  bp->~Fp64();
  TEST_REQUIRE_EQ(a, -1.561e20f);
  TEST_REQUIRE_EQ(b, 3.6718e11);
  // Technically we are accessing a destructed object here
  TEST_REQUIRE_EQ(*ap, -1.561e20f);
  TEST_REQUIRE_EQ(*bp, 3.6718e11);
}

// Check that T and U are compatible types, arithmetic to type V, comparable
template <typename T, typename U, typename V>
consteval bool checkArithmeticAndCompareSame() {
  return requires(T a, U b) {
    { a + b } -> std::same_as<V>;
    { a - b } -> std::same_as<V>;
    { a * b } -> std::same_as<V>;
    { a / b } -> std::same_as<V>;
    { a == b } -> std::same_as<bool>;
    { a != b } -> std::same_as<bool>;
    { a < b } -> std::same_as<bool>;
    { a > b } -> std::same_as<bool>;
    { a <= b } -> std::same_as<bool>;
    { a >= b } -> std::same_as<bool>;
    { a <=> b } -> std::same_as<std::partial_ordering>;
  };
}

// Incompatible types should have a compile error for all these operations
template <typename T, typename U>
consteval bool checkArithmeticAndCompareDiff() {
  return !requires(T a, U b) { a + b; } && !requires(T a, U b) { a - b; } &&
         !requires(T a, U b) { a * b; } && !requires(T a, U b) { a / b; } &&
         !requires(T a, U b) { a == b; } && !requires(T a, U b) { a != b; } &&
         !requires(T a, U b) { a < b; } && !requires(T a, U b) { a > b; } &&
         !requires(T a, U b) { a <= b; } && !requires(T a, U b) { a >= b; } &&
         !requires(T a, U b) { a <=> b; };
}

static_assert(checkArithmeticAndCompareSame<Fp32, Fp32, Fp32>());
static_assert(checkArithmeticAndCompareSame<Fp32, float, Fp32>());
static_assert(checkArithmeticAndCompareSame<float, Fp32, Fp32>());
static_assert(checkArithmeticAndCompareSame<float, float, float>());
static_assert(checkArithmeticAndCompareSame<Fp64, Fp64, Fp64>());
static_assert(checkArithmeticAndCompareSame<Fp64, double, Fp64>());
static_assert(checkArithmeticAndCompareSame<double, Fp64, Fp64>());
static_assert(checkArithmeticAndCompareSame<double, double, double>());

static_assert(checkArithmeticAndCompareDiff<Fp32, Fp64>());
static_assert(checkArithmeticAndCompareDiff<Fp32, double>());
static_assert(checkArithmeticAndCompareDiff<Fp64, Fp32>());
static_assert(checkArithmeticAndCompareDiff<Fp64, float>());
static_assert(checkArithmeticAndCompareDiff<float, Fp64>());
static_assert(checkArithmeticAndCompareDiff<double, Fp32>());

// Check unary operators
static_assert(requires(Fp32 a) {
  { +a } -> std::same_as<Fp32>;
  { -a } -> std::same_as<Fp32>;
});
static_assert(requires(Fp64 a) {
  { +a } -> std::same_as<Fp64>;
  { -a } -> std::same_as<Fp64>;
});

TEST_CREATE_FAST(add) {
  TEST_REQUIRE_EQ(Fp32(3.25f) + Fp32(-1.5f), 1.75f);
  TEST_REQUIRE_EQ(Fp32(14.0f) + Fp32(6.5f), 20.5f);
  TEST_REQUIRE_EQ(Fp64(-5.5) + Fp64(-0.5), -6.0);
  TEST_REQUIRE_EQ(Fp64(-4.12) + Fp64(4.12), 0.0);
}

TEST_CREATE_FAST(sub) {
  TEST_REQUIRE_EQ(Fp32(7.5f) - Fp32(2.5f), 5.0f);
  TEST_REQUIRE_EQ(Fp32(1.75f) - Fp32(1.875f), -0.125f);
  TEST_REQUIRE_EQ(Fp64(5.0) - Fp64(6.0), -1.0);
  TEST_REQUIRE_EQ(Fp64(-3.25) - Fp64(-1.875), -1.375);
}

TEST_CREATE_FAST(mul) {
  TEST_REQUIRE_EQ(Fp32(6.1f) * Fp32(0.0f), 0.0f);
  TEST_REQUIRE_EQ(Fp32(-1.5f) * Fp32(1.5f), -2.25f);
  TEST_REQUIRE_EQ(Fp64(-3.5) * Fp64(-2.5), 8.75);
  TEST_REQUIRE_EQ(Fp64(0.0) * Fp64(-81.7), 0.0);
}

TEST_CREATE_FAST(div) {
  TEST_REQUIRE_EQ(Fp32(8.92f) / Fp32(-8.92f), -1.0f);
  TEST_REQUIRE_EQ(Fp32(7.5f) / Fp32(2.0f), 3.75f);
  TEST_REQUIRE_EQ(Fp64(-10.5) / Fp64(-3.5), 3.0);
  TEST_REQUIRE_EQ(Fp64(12.5) / Fp64(-0.25), -50.0);
}

TEST_CREATE_FAST(unary) {
  TEST_REQUIRE_EQ(+Fp32(5.9f), 5.9f);
  TEST_REQUIRE_EQ(+Fp32(-18.71f), -18.71f);
  TEST_REQUIRE_EQ(+Fp64(-12.1), -12.1);
  TEST_REQUIRE_EQ(+Fp64(61.0), 61.0);
  TEST_REQUIRE_EQ(-Fp32(5.9f), -5.9f);
  TEST_REQUIRE_EQ(-Fp32(-18.71f), 18.71f);
  TEST_REQUIRE_EQ(-Fp64(-12.1), 12.1);
  TEST_REQUIRE_EQ(-Fp64(61.0), -61.0);
}

TEST_CREATE_FAST(compare) {
  TEST_REQUIRE_EQ(Fp32(99.1f), Fp32(99.1f));
  TEST_REQUIRE_EQ(Fp64(-102.4), Fp64(-102.4));
  TEST_REQUIRE_NE(Fp32(0.0f), Fp32(-1.1f));
  TEST_REQUIRE_NE(Fp64(1.4), Fp64(1.5));
  TEST_REQUIRE_LT(Fp32(-1.0f), Fp32(1.0f));
  TEST_REQUIRE_LT(Fp64(-0.00003), Fp64(-0.00002));
  TEST_REQUIRE_GT(Fp32(15.0f), Fp32(14.0f));
  TEST_REQUIRE_GT(Fp64(-12.0), Fp64(-12.1));
  TEST_REQUIRE_LE(Fp32(-7.1f), Fp32(-6.1f));
  TEST_REQUIRE_LE(Fp32(6.8f), Fp32(6.8f));
  TEST_REQUIRE_LE(Fp64(14.7), Fp64(15.4));
  TEST_REQUIRE_LE(Fp64(-1023.9), Fp64(-1023.9));
  TEST_REQUIRE_GE(Fp32(7.12f), Fp32(7.11f));
  TEST_REQUIRE_GE(Fp32(-501.0f), Fp32(-501.0f));
  TEST_REQUIRE_GE(Fp64(19.1), Fp64(-19.1));
  TEST_REQUIRE_GE(Fp64(8.2), Fp64(8.2));
}
