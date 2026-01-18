#include <tkoz/ff/PointData.hpp>

namespace tkoz::ff {

// TODO implement the JSON functions here instead of in the header

} // namespace tkoz::ff

#if TEST

#include <tkoz/SRTest.hpp>

#include <span>
#include <utility>

namespace tkoz::ff {

// Compile time checks on size and alignment. These should never change and are
// important to performance.
namespace {

template <std::size_t N>
[[nodiscard]] inline consteval auto staticAssertPointSize() noexcept -> bool {
  return sizeof(PointData<N, float>) == N * sizeof(float) &&
         sizeof(PointData<N, double>) == N * sizeof(double);
}

template <std::size_t N>
[[nodiscard]] inline consteval auto staticAssertPointAlign() noexcept -> bool {
  return alignof(PointData<N, float>) == alignof(float) &&
         alignof(PointData<N, double>) == alignof(double);
}

template <std::size_t... Ns>
[[nodiscard]] inline consteval auto staticAssertPointAllSizes() noexcept
    -> bool {
  return (staticAssertPointSize<Ns>() && ...);
}

template <std::size_t... Ns>
[[nodiscard]] inline consteval auto staticAssertPointAllAligns() noexcept
    -> bool {
  return (staticAssertPointAlign<Ns>() && ...);
}

// Check some compile time expectations for a few dimensions
static_assert(staticAssertPointAllSizes<1, 2, 3, 4, 5>());
static_assert(staticAssertPointAllAligns<1, 2, 3, 5, 5>());

} // namespace

} // namespace tkoz::ff

TEST_CREATE_FAST(ctors) {
  using tkoz::ff::PointData;

  // We use placement new to check the memory directly

  // Default leaves memory uninitialized
  {
    float data[3] = {16.1583f, -3.881f, -561.0f};
    auto *point = new (data) PointData<3, float>;
    std::ignore = point;
    TEST_REQUIRE_EQ(data[0], 16.1583f);
    TEST_REQUIRE_EQ(data[1], -3.881f);
    TEST_REQUIRE_EQ(data[2], -561.0f);
  }

  // The zero point should overwrite the memory with zero
  {
    float data[3] = {-999.0f, -9999.0f, -99999.0f};
    *(reinterpret_cast<PointData<3, float> *>(data)) =
        PointData<3, float>::zero();
    TEST_REQUIRE_EQ(data[0], 0.0f);
    TEST_REQUIRE_EQ(data[0], 0.0f);
    TEST_REQUIRE_EQ(data[0], 0.0f);
  }

  // Number arg sequence
  {
    double data[4];
    auto *point = new (data) PointData<4, double>(-3.14, 3.14, -6.28, 6.28);
    std::ignore = point;
    TEST_REQUIRE_EQ(data[0], -3.14);
    TEST_REQUIRE_EQ(data[1], 3.14);
    TEST_REQUIRE_EQ(data[2], -6.28);
    TEST_REQUIRE_EQ(data[3], 6.28);
  }

  // From array
  {
    std::array<double, 4> array{9.8, 13.8, -12.1, 299.79};
    double data[4];
    auto *point = new (data) PointData<4, double>(array);
    std::ignore = point;
    TEST_REQUIRE_EQ(data[0], 9.8);
    TEST_REQUIRE_EQ(data[1], 13.8);
    TEST_REQUIRE_EQ(data[2], -12.1);
    TEST_REQUIRE_EQ(data[3], 299.79);
  }

  // From span
  {
    float data[5] = {13.0, 21.0, 34.0, 55.0, 89.0};
    std::span span(data);
    float data2[5];
    auto *point = new (data2) PointData<5, float>(span);
    std::ignore = point;
    TEST_REQUIRE_EQ(data2[0], 13.0);
    TEST_REQUIRE_EQ(data2[1], 21.0);
    TEST_REQUIRE_EQ(data2[2], 34.0);
    TEST_REQUIRE_EQ(data2[3], 55.0);
    TEST_REQUIRE_EQ(data2[4], 89.0);
  }

  // From C array
  {
    double data[2] = {1.618033988749895, 2.718281828459045};
    double data2[2];
    auto *point = new (data2) PointData<2, double>(data);
    std::ignore = point;
    TEST_REQUIRE_EQ(data2[0], 1.618033988749895);
    TEST_REQUIRE_EQ(data2[1], 2.718281828459045);
  }
}

TEST_CREATE_FAST(access) {
  using tkoz::ff::PointData;

  // Data array and pointer
  {
    PointData<7, float> a;
    PointData<9, double> b;
    TEST_REQUIRE_EQ(static_cast<void *>(&a.data()), static_cast<void *>(&a));
    TEST_REQUIRE_EQ(static_cast<void *>(&b.data()), static_cast<void *>(&b));
    TEST_REQUIRE_EQ(static_cast<void *>(a.dataPtr()), static_cast<void *>(&a));
    TEST_REQUIRE_EQ(static_cast<void *>(b.dataPtr()), static_cast<void *>(&b));
    // And the const versions
    auto const &ac = a;
    auto const &bc = b;
    TEST_REQUIRE_EQ(static_cast<void const *>(&ac.data()),
                    static_cast<void const *>(&ac));
    TEST_REQUIRE_EQ(static_cast<void const *>(&bc.data()),
                    static_cast<void const *>(&bc));
    TEST_REQUIRE_EQ(static_cast<void const *>(ac.dataPtr()),
                    static_cast<void const *>(&ac));
    TEST_REQUIRE_EQ(static_cast<void const *>(bc.dataPtr()),
                    static_cast<void const *>(&bc));
  }

  // Element access (operator[])
  {
    PointData<4, float> a(-1.1f, 2.2f, -3.3f, 4.4f);
    PointData<8, double> b(3.0, 3.1, 3.14, 3.141, 3.1415, 3.14159, 3.141592,
                           3.1415926);
    TEST_REQUIRE_EQ(a[0], -1.1f);
    TEST_REQUIRE_EQ(a[1], 2.2f);
    TEST_REQUIRE_EQ(a[2], -3.3f);
    TEST_REQUIRE_EQ(a[3], 4.4f);
    TEST_REQUIRE_EQ(b[0], 3.0);
    TEST_REQUIRE_EQ(b[1], 3.1);
    TEST_REQUIRE_EQ(b[2], 3.14);
    TEST_REQUIRE_EQ(b[3], 3.141);
    TEST_REQUIRE_EQ(b[4], 3.1415);
    TEST_REQUIRE_EQ(b[5], 3.14159);
    TEST_REQUIRE_EQ(b[6], 3.141592);
    TEST_REQUIRE_EQ(b[7], 3.1415926);
    // Reassign some
    a[1] = 1.5f;
    a[3] = 14.0f;
    b[2] = -1.5;
    b[6] = 15.5f;
    TEST_REQUIRE_EQ(a[1], 1.5f);
    TEST_REQUIRE_EQ(a[3], 14.0f);
    TEST_REQUIRE_EQ(b[2], -1.5);
    TEST_REQUIRE_EQ(b[6], 15.5);
    // And const versions
    auto const &ac = a;
    auto const &bc = b;
    TEST_REQUIRE_EQ(ac[0], -1.1f);
    TEST_REQUIRE_EQ(ac[1], 1.5f);
    TEST_REQUIRE_EQ(ac[2], -3.3f);
    TEST_REQUIRE_EQ(ac[3], 14.0f);
    TEST_REQUIRE_EQ(bc[0], 3.0);
    TEST_REQUIRE_EQ(bc[1], 3.1);
    TEST_REQUIRE_EQ(bc[2], -1.5);
    TEST_REQUIRE_EQ(bc[3], 3.141);
    TEST_REQUIRE_EQ(bc[4], 3.1415);
    TEST_REQUIRE_EQ(bc[5], 3.14159);
    TEST_REQUIRE_EQ(bc[6], 15.5);
    TEST_REQUIRE_EQ(bc[7], 3.1415926);
  }

  // Element access (run time at)
  {
    PointData<3, double> a(1.01, 1.03, 1.05);
    PointData<1, float> b(-10.0f);
    TEST_REQUIRE_EQ(a.at(0), 1.01);
    TEST_REQUIRE_EQ(a.at(1), 1.03);
    TEST_REQUIRE_EQ(a.at(2), 1.05);
    TEST_REQUIRE_EQ(b.at(0), -10.0f);
    TEST_REQUIRE_THROW(a.at(3), std::out_of_range);
    TEST_REQUIRE_THROW(b.at(1), std::out_of_range);
    TEST_REQUIRE_THROW_ANY(a.at(-1));
    TEST_REQUIRE_THROW_ANY(b.at(-1));
    // Reassign some
    a.at(2) = 1.07;
    b.at(0) = 1.09f;
    TEST_REQUIRE_EQ(a.at(2), 1.07);
    TEST_REQUIRE_EQ(b.at(0), 1.09f);
    // And const versions
    auto const &ac = a;
    auto const &bc = b;
    TEST_REQUIRE_EQ(ac.at(0), 1.01);
    TEST_REQUIRE_EQ(ac.at(1), 1.03);
    TEST_REQUIRE_EQ(ac.at(2), 1.07);
    TEST_REQUIRE_EQ(bc.at(0), 1.09f);
    TEST_REQUIRE_THROW_ANY(ac.at(3));
    TEST_REQUIRE_THROW_ANY(bc.at(1));
    TEST_REQUIRE_THROW(ac.at(-1), std::out_of_range);
    TEST_REQUIRE_THROW(bc.at(-1), std::out_of_range);
  }

  // Element access (compile time at)
  {
    PointData<2, double> a(3.14, 2.718);
    PointData<3, float> b(299792458.0f, 9.8f, 6.02e23f);
    TEST_REQUIRE_EQ(a.at<0>(), 3.14);
    TEST_REQUIRE_EQ(a.at<1>(), 2.718);
    TEST_REQUIRE_EQ(b.at<0>(), 299792458.0f);
    TEST_REQUIRE_EQ(b.at<1>(), 9.8f);
    TEST_REQUIRE_EQ(b.at<2>(), 6.02e23f);
    // Reassign some
    a.at<0>() = 3.1415;
    b.at<2>() = 3.1415f;
    TEST_REQUIRE_EQ(a.at<0>(), 3.1415);
    TEST_REQUIRE_EQ(b.at<2>(), 3.1415f);
    // And const versions
    auto const &ac = a;
    auto const &bc = b;
    TEST_REQUIRE_EQ(ac.at<0>(), 3.1415);
    TEST_REQUIRE_EQ(ac.at<1>(), 2.718);
    TEST_REQUIRE_EQ(bc.at<0>(), 299792458.0f);
    TEST_REQUIRE_EQ(bc.at<1>(), 9.8f);
    TEST_REQUIRE_EQ(bc.at<2>(), 3.1415f);
  }
}

template <typename IterT, typename ValueT>
concept cCanAssign = requires(IterT i, ValueT v) { *i = v; };

TEST_CREATE_FAST(iterate) {
  using tkoz::ff::PointData;

  // Forward direction
  {
    PointData<6, float> a(6.9f, 9.6f, -1.0f, -2.0f, -3.0f, -4.0f);
    PointData<5, double> b(1.0, 0.5, 0.33333333333333333333, 0.25, 0.2);

    TEST_REQUIRE_EQ(*a.begin(), 6.9f);
    TEST_REQUIRE_EQ(a.begin() + 6, a.end());
    TEST_REQUIRE_EQ(*b.begin(), 1.0);
    TEST_REQUIRE_EQ(b.begin() + 5, b.end());
    TEST_REQUIRE_EQ(*a.cbegin(), 6.9f);
    TEST_REQUIRE_EQ(a.cbegin() + 6, a.cend());
    TEST_REQUIRE_EQ(*b.cbegin(), 1.0);
    TEST_REQUIRE_EQ(b.cbegin() + 5, b.cend());

    static_assert(cCanAssign<decltype(a.begin()), float>);
    static_assert(cCanAssign<decltype(b.end()), double>);
    static_assert(!cCanAssign<decltype(a.cbegin()), float>);
    static_assert(!cCanAssign<decltype(b.cend()), double>);

    std::vector<float> av;
    std::size_t i = 0;
    for (float &f : a) {
      av.push_back(f);
      f = static_cast<float>(i * i);
      ++i;
    }
    TEST_REQUIRE_EQ(
        av, (std::vector<float>{6.9f, 9.6f, -1.0f, -2.0f, -3.0f, -4.0f}));
    TEST_REQUIRE_EQ(
        a.data(), (std::array<float, 6>{0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f}));

    std::vector<double> bv;
    for (double const &d : b) {
      bv.push_back(d);
    }
    TEST_REQUIRE_EQ(
        bv, (std::vector<double>{1.0, 0.5, 0.33333333333333333333, 0.25, 0.2}));

    // And with const references
    auto const &ac = a;
    auto const &bc = b;

    TEST_REQUIRE_EQ(*ac.begin(), 0.0f);
    TEST_REQUIRE_EQ(ac.begin() + 6, ac.end());
    TEST_REQUIRE_EQ(*bc.begin(), 1.0);
    TEST_REQUIRE_EQ(bc.begin() + 5, bc.end());
    TEST_REQUIRE_EQ(*ac.cbegin(), 0.0f);
    TEST_REQUIRE_EQ(ac.cbegin() + 6, ac.cend());
    TEST_REQUIRE_EQ(*bc.cbegin(), 1.0);
    TEST_REQUIRE_EQ(bc.cbegin() + 5, bc.cend());

    static_assert(!cCanAssign<decltype(ac.begin()), float>);
    static_assert(!cCanAssign<decltype(ac.end()), float>);
    static_assert(!cCanAssign<decltype(bc.cbegin()), double>);
    static_assert(!cCanAssign<decltype(bc.cend()), double>);
  }

  // Reverse direction
  {
    PointData<2, float> a(10.0f, 20.0f);
    PointData<3, double> b(1.0, 1.5, 2.0);

    TEST_REQUIRE_EQ(*a.rbegin(), 20.0f);
    TEST_REQUIRE_EQ(a.rbegin() + 2, a.rend());
    TEST_REQUIRE_EQ(*b.crbegin(), 2.0);
    TEST_REQUIRE_EQ(b.crbegin() + 3, b.crend());

    static_assert(cCanAssign<decltype(a.rbegin()), float>);
    static_assert(cCanAssign<decltype(b.rend()), double>);
    static_assert(!cCanAssign<decltype(a.crbegin()), float>);
    static_assert(!cCanAssign<decltype(b.crend()), double>);

    std::vector<float> av;
    std::vector<double> bv;
    std::size_t ai = 16;
    for (auto iter = a.rbegin(); iter != a.rend(); ++iter) {
      av.push_back(*iter);
      *iter = static_cast<float>(++ai);
    }
    for (auto iter = b.crbegin(); iter != b.crend(); ++iter) {
      bv.push_back(*iter);
    }
    TEST_REQUIRE_EQ(av, (std::vector<float>{20.0f, 10.0f}));
    TEST_REQUIRE_EQ(a.data(), (std::array<float, 2>{18.0f, 17.0f}));
    TEST_REQUIRE_EQ(bv, (std::vector<double>{2.0, 1.5, 1.0}));

    // And with const references
    auto const &ac = a;
    auto const &bc = b;

    TEST_REQUIRE_EQ(*ac.rbegin(), 17.0f);
    TEST_REQUIRE_EQ(*bc.crbegin(), 2.0);
    TEST_REQUIRE_EQ(ac.rbegin() + 2, ac.rend());
    TEST_REQUIRE_EQ(bc.crbegin() + 3, bc.crend());

    static_assert(!cCanAssign<decltype(ac.rbegin()), float>);
    static_assert(!cCanAssign<decltype(ac.rend()), float>);
    static_assert(!cCanAssign<decltype(bc.crbegin()), double>);
    static_assert(!cCanAssign<decltype(bc.crend()), double>);
  }
}

#endif // TEST
