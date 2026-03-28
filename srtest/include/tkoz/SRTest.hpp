/// SRTest - statically registered test library
///
/// Tests are meant to be defined in their own project to separate production
/// code from test code. They are registered to an executable by statically
/// linking to the test libraries.
///
/// Do not use anything in the \c tkoz::srtest namespace directly. This is
/// currently internal and subject to change. Write tests using the \c TEST_*
/// macros which are considered the only public interface of the library.

#pragma once

#include <bitset>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

/// Statically registered test library. The contents of this namespace are not
/// intended to be used by client code and may change. To create tests and test
/// conditions, use the \c TEST_* macros.
///
/// The only thing stopping us from having a unit test library without macros
/// is some nice things like stringifying expressions passed to the condition
/// checking macros.
namespace tkoz::srtest {

using TestFunction = void (*)();

/// \brief Collection of tags assigned to a test case.
class TestTags final {
private:
  static constexpr std::size_t cMaxTags = 2;
  std::bitset<cMaxTags> mTagBits;
  inline TestTags() = default;

public:
  /// Tags available for a test. We intentionally avoid enum class so they are
  /// conveniently accessible within this class.
  /// Note: TAG_COUNT must be at the end. Use automatic enum values only.
  /// TODO consider the "x macro" design pattern. The main tag list would be
  /// defined by a macro elsewhere and inserted here with TAG_COUNT after it.
  enum TagEnum : std::uint16_t { FAST, SLOW, TAG_COUNT };

  static_assert(static_cast<std::size_t>(TAG_COUNT) == cMaxTags);

  /// Get a string name for a tag.
  /// \return A string name for a tag.
  [[nodiscard]] static auto tagString(TagEnum aTag) -> std::string const &;

  /// Create a compile time object for tags assigned to a test.
  /// \tparam cTagsT The tags specified
  template <TagEnum... cTagsT>
  [[nodiscard]] static inline consteval auto create() noexcept -> TestTags {
    static_assert(((cTagsT != TAG_COUNT) && ...),
                  "TAG_COUNT is reserved and not a valid test tag");
    TestTags result;
    (result.mTagBits.set(static_cast<std::size_t>(cTagsT)), ...);
    return result;
  }

  /// Create a tag set for a test case. This is done at runtime unlike
  /// \c create() .
  /// \tparam TestTagT The tag type (all must be \c TagEnum ).
  /// \param tags The tag values.
  // template <typename... TestTagT>
  //   requires(std::is_same_v<TestTagT, TagEnum> && ...)
  //[[nodiscard]] inline constexpr TestTags(TestTagT... tags) noexcept {
  //   (mTagBits.set(static_cast<std::size_t>(tags)), ...);
  // }

  /// Test if a tag is set.
  /// \param aTag A tag to test for.
  /// \return True if \c tag is set.
  [[nodiscard]] inline constexpr auto hasTag(TagEnum aTag) const noexcept
      -> bool {
    return mTagBits.test(static_cast<std::size_t>(aTag));
  }

  /// List all tags which are set. They are ordered as they appear in the enum
  /// declaration.
  /// \return All tags which are set.
  [[nodiscard]] auto allTags() const noexcept -> std::vector<TagEnum>;
};

/// Tests are created inside this namespace. We make the tags accessible without
/// namespace path for convenience of the test creation macros.
namespace tests {
using enum ::tkoz::srtest::TestTags::TagEnum;
} // namespace tests

/// Remove the repo root path from a test filename.
/// \param aFullPath The full file path from the \c __FILE__ macro
/// \return The path with the repo root removed.
[[nodiscard]] auto testFilePath(std::string_view aFullPath) -> std::string;

/// \brief The data associated with a single test.
struct TestCaseInfo final {
  TestCaseInfo() = delete;

  /// Create info for a test case.
  /// \param aFunc The function to run the test.
  /// \param aName The name of the test.
  /// \param aFile The file containing the test.
  /// \param aLine The line number of the test.
  /// \param aTags The tags assigned to the test.
  [[nodiscard]] inline TestCaseInfo(TestFunction aFunc, std::string aName,
                                    std::string const &aFile, std::size_t aLine,
                                    TestTags aTags)
      : mFunc(aFunc), mName(std::move(aName)), mFile(testFilePath(aFile)),
        mLine(aLine), mTags(aTags) {}

  TestCaseInfo(TestCaseInfo const &) = default;
  TestCaseInfo(TestCaseInfo &&) = default;

  /// Call the test function.
  inline void run() const { mFunc(); }

  /// Function object to run the test.
  TestFunction const mFunc;

  /// Name/identifier for the test.
  std::string const mName;

  /// File containing the test
  std::string const mFile;

  /// Line where test is defined
  std::size_t const mLine;

  /// Test tags
  TestTags const mTags;

  /// Define the canonical ordering of tests to be first by file, then by the
  /// order they are defined within a file (line number).
  friend auto operator<=>(TestCaseInfo const &aLeft,
                          TestCaseInfo const &aRight) noexcept
      -> std::strong_ordering;
};

[[nodiscard]] auto operator<=>(TestCaseInfo const &aLeft,
                               TestCaseInfo const &aRight) noexcept
    -> std::strong_ordering;

/// \brief The registry storing all statically registered tests.
class TestRegistry final {
private:
  /// All tests that have been registered.
  std::vector<std::unique_ptr<TestCaseInfo>> mAllTests;

  // This class is a singleton.
  TestRegistry() = default;

public:
  TestRegistry(TestRegistry const &) = delete;
  TestRegistry(TestRegistry &&) = delete;
  TestRegistry &operator=(TestRegistry const &) = delete;
  TestRegistry &operator=(TestRegistry &&) = delete;

  /// \return The singleton instance keeping track of available tests.
  [[nodiscard]] static auto instance() noexcept -> TestRegistry &;

  /// \return All tests contained in the registry.
  [[nodiscard]] inline auto allTests() const noexcept
      -> std::vector<std::unique_ptr<TestCaseInfo>> const & {
    return mAllTests;
  }

  /// \return Begin iterator for all registered tests.
  [[nodiscard]] inline auto begin() const noexcept { return mAllTests.begin(); }

  /// \return End iterator for all registered tests.
  [[nodiscard]] inline auto end() const noexcept { return mAllTests.end(); }

  /// \return Number of registered tests.
  [[nodiscard]] inline auto size() const noexcept -> std::size_t {
    return mAllTests.size();
  }

  /// \return True if there are no registered tests.
  [[nodiscard]] inline auto empty() const noexcept -> bool {
    return mAllTests.empty();
  }

  /// Add a test to the registry.
  /// \param aFunc Function object for running the test.
  /// \param aName Name/identifier for the test.
  /// \param aFile The file containing the test.
  /// \param aLine Line where the test is defined.
  /// \param aTags Tags assigned to the test.
  inline void addTest(TestFunction aFunc, std::string aName, std::string aFile,
                      std::size_t aLine, TestTags aTags) {
    mAllTests.push_back(std::make_unique<TestCaseInfo>(
        aFunc, std::move(aName), std::move(aFile), aLine, aTags));
  }
};

/// Register a test with a function call rather than a static object.
/// For convenience, usually you would use the TEST_CREATE macro.
/// \param aFunc A function object used to run the test.
/// \param aName A string identifier for the test.
/// \param aFile File containing the test.
/// \param aLine Line where test is defined.
/// \param aTags Tags assigned to the test.
inline void registerTest(TestFunction aFunc, std::string aName,
                         std::string aFile, std::size_t aLine, TestTags aTags) {
  TestRegistry::instance().addTest(aFunc, std::move(aName), std::move(aFile),
                                   aLine, aTags);
}

/// \brief A class for registering tests. Create a static instance of this
/// class and its constructor registers the test to the TestRegistry.
/// For convenience, usually you would use the TEST_CREATE macro.
class TestRegistrar final {
public:
  TestRegistrar() = delete;

  /// This constructor adds a test to the test registry.
  /// \param aFunc The function object to run the test.
  /// \param aName A name/identifier for the test.
  /// \param aFile File where test is defined.
  /// \param aLine Line where test is defined.
  /// \param aTags Tags assigned to the test.
  TestRegistrar(TestFunction aFunc, std::string aName, std::string aFile,
                std::size_t aLine, TestTags aTags) {
    registerTest(aFunc, std::move(aName), std::move(aFile), aLine, aTags);
  }
};

/// \brief Error type thrown by test macros. Does not inherit \c std::exception
/// to force differentiating it.
class TestFailure final {
private:
  std::string const mMessage;

public:
  [[nodiscard]] inline TestFailure(std::string message)
      : mMessage(std::move(message)) {}
  ~TestFailure() = default;

  [[nodiscard]] inline auto message() const noexcept -> std::string const & {
    return mMessage;
  }
};

/// Helper to ensure the std::source_location is correctly provided.
/// By using \c consteval , we require the default usage.
/// \param aSrcLoc Source location object. Use default only.
[[nodiscard]] inline consteval auto sourceLocation(
    std::source_location aSrcLoc = std::source_location::current()) noexcept
    -> std::source_location {
  return aSrcLoc;
}

/// Throw a test failure.
/// \param aMessage Message for further details about the test failure.
/// \param aSrcLoc Source location object (use default value)>
/// \throw TestFailure Always.
[[noreturn]] void throwFailure(std::string_view aMessage,
                               std::source_location aSrcLoc = sourceLocation());

/// Add a message to the thread_local storage for test results.
/// \param aMessage The message.
/// \param aFailureOnly True if it should only be printed on test failure.
void addMessage(std::string aMessage, bool aFailureOnly);

/// Clears the thread_local message storage for test results.
void clearMessages();

/// Require a condition to be true, fails the test if false.
/// \param aCondition A boolean testable.
/// \param aFalseMsg Message to use for the exception when \a aCondition is
/// false.
/// \param aSrcLoc Source location object (use default value).
/// \throw TestFailure If \a aCondition evaluates to false.
/// \note This function may not work as nicely with compound boolean testables
/// like (a >= 0 && a < n) because it only considers the final value.
/// \note A fundamental limitation (before reflection) is that purely macro free
/// libraries cannot turn the expression into a string, so macros are still very
/// helpful for unit test library design.
void requireCondition(bool aCondition, std::string_view aFalseMsg = "",
                      std::source_location aSrcLoc = sourceLocation());

/// Helper to demangle names from the GCC/Clang ABI for current exception.
/// If not possible to get type information, returns `"unknown type"`.
/// If somehow we compile this with an unsupported compiler, it falls back to
/// simply returning the name without demangling.
/// \param aTypeInfo The \c std::type_info object from
/// \c abi::__cxa_current_exception_type (or nullptr for fallback)
/// \return C string for the demangled name attempt.
[[nodiscard]] auto typeName(std::type_info const *aTypeInfo) -> std::string;

// Helper functions for the require (no)throw stuff.
namespace internal {
[[noreturn]] void requireThrowExcHelper(std::exception_ptr const &aExcPtr,
                                        std::string_view aExpr,
                                        std::string_view aCustMsg,
                                        std::string_view aExpectedTypeName,
                                        std::source_location aSrcLoc);
[[noreturn]] void requireThrowAnyHelper(std::string_view aExpr,
                                        std::string_view aCustMsg,
                                        std::source_location aSrcLoc);
[[noreturn]] void requireNoThrowHelper(std::exception_ptr const &aExcPtr,
                                       std::string_view aExpr,
                                       std::string_view aCustMsg,
                                       std::source_location aSrcLoc);
} // namespace internal

/// Require an exception of specified type to occur.
/// \param aFunc Function to run to expect an exception from.
/// \param aExpr Expression string to put into the formatted error messages.
/// \param aCustMsg Custom message to use instead of the default ones.
/// \param aSrcLoc Source location object (use default value).
/// \tparam ExcTypeT Exception type expected to be thrown.
/// \tparam FuncT Type of function which is run to test for an exception.
/// \throw TestFailure If an exception of incorrect type is thrown or no
/// exception is thrown.
template <typename ExcTypeT, typename FuncT>
inline void requireThrowExc(FuncT &&aFunc, std::string_view aExpr,
                            std::string_view aCustMsg = "",
                            std::source_location aSrcLoc = sourceLocation()) {
  std::exception_ptr lExcPtr = nullptr;
  try {
    std::forward<FuncT>(aFunc)();
    // Call helper with nullptr (no exception thrown when one was expected)
  } catch (ExcTypeT const &) {
    return; // Successful
  } catch (...) {
    lExcPtr = std::current_exception();
    // Call helper with exception pointer (wrong type thrown)
  }
  internal::requireThrowExcHelper(lExcPtr, aExpr, aCustMsg,
                                  typeName(&typeid(ExcTypeT)), aSrcLoc);
}

/// Require an exception to be thrown. Type of exception is not checked.
/// \param aFunc Function to run to expect an exception from.
/// \param aExpr An optional message for additional information.
/// \param aCustMsg Custom message to use instead of the default ones.
/// \param aSrcLoc Source location object (use default value).
/// \tparam FuncT Type of function which is run to test for an exception.
/// \throw TestFailure If no except is thrown.
template <typename FuncT>
inline void requireThrowAny(FuncT &&aFunc, std::string_view aExpr,
                            std::string_view aCustMsg = "",
                            std::source_location aSrcLoc = sourceLocation()) {
  try {
    std::forward<FuncT>(aFunc)();
  } catch (...) {
    return; // Successful
  }
  internal::requireThrowAnyHelper(aExpr, aCustMsg, aSrcLoc);
}

/// Require no exception to be thrown.
/// \param aFunc Function to run expecting no exceptions from.
/// \param aExpr An optional message for additional information.
/// \param aCustMsg Custom message to use instead of the default ones.
/// \param aSrcLoc Source location object (use default value).
/// \tparam FuncT Type of function which is run to test for no exception.
/// \throw TestFailure If any exception is thrown.
template <typename FuncT>
inline void requireNoThrow(FuncT &&aFunc, std::string_view aExpr,
                           std::string_view aCustMsg = "",
                           std::source_location aSrcLoc = sourceLocation()) {
  try {
    std::forward<FuncT>(aFunc)();
  } catch (...) {
    internal::requireNoThrowHelper(std::current_exception(), aExpr, aCustMsg,
                                   aSrcLoc);
  }
}

/// Type of relative error. We can divide by the expected value, maximum, or
/// average of the 2. Using \c cExp will give you a relative error compared to
/// the expected value. The other options are symmetric about both inputs.
enum class RelErrDiv : std::uint8_t {
  /// Divide by expected value, to compare relative difference from one value.
  cExp,
  /// Divide by max magnitude of the 2, the general recommendation.
  cMax,
  /// Divide by average magnitude of the 2, a lesser used alternative.
  cAvg
};

// Helpers for floating point error and checks
namespace internal {
template <std::floating_point> struct FpBits;
template <> struct FpBits<float> {
  using Signed = std::int32_t;
  using Unsigned = std::uint32_t;
  static_assert(sizeof(Signed) == sizeof(float));
  static_assert(sizeof(Unsigned) == sizeof(float));
};
template <> struct FpBits<double> {
  using Signed = std::int64_t;
  using Unsigned = std::uint64_t;
  static_assert(sizeof(Signed) == sizeof(double));
  static_assert(sizeof(Unsigned) == sizeof(double));
};

// Unconstrained primary templates so we can define and instantiate elsewhere

template <typename T> auto fpErrUlpImpl(T, T) -> internal::FpBits<T>::Unsigned;
template <bool, std::floating_point T>
void requireCloseAbsImpl(T, T, T, char const *, char const *, char const *,
                         std::source_location);
template <RelErrDiv, bool, std::floating_point T>
void requireCloseRelImpl(T, T, T, char const *, char const *, char const *,
                         std::source_location);
template <bool, std::floating_point T, typename U>
void requireCloseUlpImpl(T, T, U, char const *, char const *, char const *,
                         std::source_location);
template <bool, std::floating_point T>
void requireCloseImpl(T, T, T, T, char const *, char const *, char const *,
                      char const *, std::source_location);
template <bool, std::floating_point T>
void requireNearImpl(T, T, T, char const *, char const *, char const *,
                     std::source_location);

} // namespace internal

/// Absolute difference between 2 floating point numbers.
/// \param aActual The actual computed value.
/// \param aExpected The expected value.
/// \tparam T Floating point type.
/// \return The absolute difference.
template <std::floating_point T>
[[nodiscard]] inline auto fpErrAbs(T aActual, T aExpected) -> T {
  return std::abs(aActual - aExpected);
}

/// Relative difference between 2 floating point numbers, using the given
/// divisor type specified at compile time, defaulted to max magnitude.
/// \param aActual The actual computed value.
/// \param aExpected The expected value.
/// \tparam cDivT Type of divisor to use for relative error.
/// \tparam T Floating point type.
/// \return The relative difference.
template <RelErrDiv cDivT = RelErrDiv::cMax, std::floating_point T>
[[nodiscard]] inline auto fpErrRel(T aActual, T aExpected) -> T {
  if constexpr (cDivT == RelErrDiv::cExp) {
    return std::abs((aActual - aExpected) / aExpected);
  } else if constexpr (cDivT == RelErrDiv::cMax) {
    return fpErrAbs(aActual, aExpected) /
           std::max(std::abs(aActual), std::abs(aExpected));
  } else if constexpr (cDivT == RelErrDiv::cAvg) {
    return T{2.0} * fpErrAbs(aActual, aExpected) /
           (std::abs(aActual) + std::abs(aExpected));
  } else {
    static_assert(false, "unexpected relative error divisor type");
  }
}

/// Difference between floating point numbers in ULPs (units in the last place).
/// This is the number of steps between them on the floating point number line.
/// \param aActual The actual computed value.
/// \param aExpected The expected value.
/// \tparam T Floating point type.
/// \return The ULP difference.
template <std::floating_point T>
  requires(!std::is_same_v<T, long double>)
[[nodiscard]] inline auto fpErrUlp(T aActual, T aExpected)
    -> internal::FpBits<T>::Unsigned {
  return internal::fpErrUlpImpl<T>(aActual, aExpected);
}

/// Check the absolute error against a given error bound.
/// \param aActual The actual computed value.
/// \param aExpected The expected result.
/// \param aTolerance The tolerance.
/// \param aActualStr Stringified expression for actual error.
/// \param aExpectedStr Stringified expression for expected result.
/// \param aToleranceStr Stringified expression for tolerance.
/// \param aSrcLoc Source location object (use default value).
/// \tparam cIncludeEqualT If the error is allowed to be equal to the bound.
/// \tparam T Floating point type.
/// \throw TestFailure If the error tolerance is not satisfied.
template <bool cIncludeEqualT = true, std::floating_point T>
  requires(!std::is_same_v<T, long double>)
inline void requireCloseAbs(T aActual, T aExpected, T aTolerance,
                            char const *aActualStr, char const *aExpectedStr,
                            char const *aToleranceStr,
                            std::source_location aSrcLoc = sourceLocation()) {
  internal::requireCloseAbsImpl<cIncludeEqualT, T>(
      aActual, aExpected, aTolerance, aActualStr, aExpectedStr, aToleranceStr,
      aSrcLoc);
}

/// Check the relative error against a given error bound.
/// \param aActual The actual computed value.
/// \param aExpected The expected result.
/// \param aTolerance The tolerance.
/// \param aActualStr Stringified expression for actual error.
/// \param aExpectedStr Stringified expression for expected result.
/// \param aToleranceStr Stringified expression for tolerance.
/// \param aSrcLoc Source location object (use default value).
/// \tparam cDivT The division type to use for computing relative error.
/// \tparam cIncludeEqualT If the error is allowed to be equal to the bound.
/// \tparam T Floating point type.
/// \throw TestFailure If the error tolerance is not satisfied.
template <RelErrDiv cDivT = RelErrDiv::cMax, bool cIncludeEqualT = true,
          std::floating_point T>
  requires(!std::is_same_v<T, long double>)
inline void requireCloseRel(T aActual, T aExpected, T aTolerance,
                            char const *aActualStr, char const *aExpectedStr,
                            char const *aToleranceStr,
                            std::source_location aSrcLoc = sourceLocation()) {
  internal::requireCloseRelImpl<cDivT, cIncludeEqualT, T>(
      aActual, aExpected, aTolerance, aActualStr, aExpectedStr, aToleranceStr,
      aSrcLoc);
}

/// Check the ULP error against a give error bound.
/// \param aActual The actual computed value.
/// \param aExpected The expected result.
/// \param aTolerance The tolerance.
/// \param aActualStr Stringified expression for actual error.
/// \param aExpectedStr Stringified expression for expected result.
/// \param aToleranceStr Stringified expression for tolerance.
/// \param aSrcLoc Source location object (use default value).
/// \tparam cIncludeEqualT If the error is allowed to be equal to the bound.
/// \tparam T Floating point type.
/// \tparam U Tolerance type. Must match size of T.
/// \throw TestFailure If the error tolerance is not satisfied.
template <bool cIncludeEqualT = true, std::floating_point T, typename U>
  requires(!std::is_same_v<T, long double> &&
           std::is_same_v<U, typename internal::FpBits<T>::Unsigned>)
inline void requireCloseUlp(T aActual, T aExpected, U aTolerance,
                            char const *aActualStr, char const *aExpectedStr,
                            char const *aToleranceStr,
                            std::source_location aSrcLoc = sourceLocation()) {
  internal::requireCloseUlpImpl<cIncludeEqualT, T, U>(
      aActual, aExpected, aTolerance, aActualStr, aExpectedStr, aToleranceStr,
      aSrcLoc);
}

/// Perform a robust check for nearly equal floating point values using both
/// provided absolute and relative tolerances. For type safety, we require all
/// args to be the same type. This is the same method used by \c math.isclose
/// in the Python math module.
/// \param aActual The actual computed value.
/// \param aExpected The expected result.
/// \param aRelTol The relative tolerance.
/// \param aAbsTol The absolute tolerance.
/// \param aActualStr Stringified expression for actual error.
/// \param aExpectedStr Stringified expression for expected result.
/// \param aRelTolStr Stringified expression for relative tolerance.
/// \param aAbsTolStr Stringified expression for absolute tolerance.
/// \param aSrcLoc Source location object (use default value).
/// \tparam cIncludeEqualT If the error is allowed to be equal to the bound.
/// \tparam T Floating point type.
/// \throw TestFailure If the error tolerance is not satisfied.
template <bool cIncludeEqualT = true, std::floating_point T>
  requires(!std::is_same_v<T, long double>)
inline void requireClose(T aActual, T aExpected, T aRelTol, T aAbsTol,
                         char const *aActualStr, char const *aExpectedStr,
                         char const *aRelTolStr, char const *aAbsTolStr,
                         std::source_location aSrcLoc = sourceLocation()) {
  internal::requireCloseImpl<cIncludeEqualT, T>(
      aActual, aExpected, aRelTol, aAbsTol, aActualStr, aExpectedStr,
      aRelTolStr, aAbsTolStr, aSrcLoc);
}

/// Perform a robust error check with a single value for the tolerance. This
/// behaves like a relative error when a number has magnitude higher than 1,
/// otherwise an absolute error for numbers near 0. It is equivalent to calling
/// \c requireClose with \c aTolerance for both \c aRelTol and \c aAbsTol .
/// \param aActual The actual computed value.
/// \param aExpected The expected result.
/// \param aTolerance The single tolerance parameter.
/// \param aActualStr Stringified expression for actual error.
/// \param aExpectedStr Stringified expression for expected result.
/// \param aToleranceStr Stringified expression for tolerance.
/// \param aSrcLoc Source location object (use default value).
/// \tparam cIncludeEqualT If the error is allowed to be equal to the bound.
/// \tparam T Floating point type.
/// \throw TestFailure If the error tolerance is not satisfied.
template <bool cIncludeEqualT = true, std::floating_point T>
  requires(!std::is_same_v<T, long double>)
inline void requireNear(T aActual, T aExpected, T aTolerance,
                        char const *aActualStr, char const *aExpectedStr,
                        char const *aToleranceStr,
                        std::source_location aSrcLoc = sourceLocation()) {
  internal::requireNearImpl<cIncludeEqualT, T>(aActual, aExpected, aTolerance,
                                               aActualStr, aExpectedStr,
                                               aToleranceStr, aSrcLoc);
}

} // namespace tkoz::srtest

/// Helper macros to expand macros properly.
#define TKOZ_SRTEST_INTERNAL_CONCAT_4_IMPL(a, b, c, d) a##b##c##d
#define TKOZ_SRTEST_INTERNAL_CONCAT_4(a, b, c, d)                              \
  TKOZ_SRTEST_INTERNAL_CONCAT_4_IMPL(a, b, c, d)

// #define TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT2(a, b) a##b
// #define TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT(a, b)
//   TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT2(a, b)

// The test function is declared and registered inside the namespace for a way
// to get the test tags to be more convenient in the interface macro. Then we
// put the test definition outside the namespace so {} goes after the macro.
#define TKOZ_SRTEST_INTERNAL_CREATE(name, cat, counter, ...)                   \
  namespace tkoz::srtest::tests {                                              \
  [[maybe_unused]] static void                                                 \
      TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_testfunc__, name, __,         \
                                    counter)();                                \
  struct [[maybe_unused]] _tkoz_srtest_names_unique__##name {};                \
  [[maybe_unused]] static ::tkoz::srtest::TestRegistrar                        \
      TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_registrar__, name, __,        \
                                    counter)(                                  \
          TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_testfunc__, name, __,     \
                                        counter),                              \
          #name, __FILE__, __LINE__,                                           \
          ::tkoz::srtest::TestTags::create<__VA_ARGS__>());                    \
  }                                                                            \
  static void ::tkoz::srtest::tests::TKOZ_SRTEST_INTERNAL_CONCAT_4(            \
      _tkoz_srtest_testfunc__, name, __, counter)()

////////////////////////////////////////////////////////////////////////////////
// These macros actually define the public interface for the test library.
////////////////////////////////////////////////////////////////////////////////

/// Create a test with the provided name (not quoted) and a curly brace {}
/// block following for the function definition.
/// Usage: TEST_CREATE(testName) { ... test code ... }
#define TEST_CREATE(name, ...)                                                 \
  TKOZ_SRTEST_INTERNAL_CREATE(name, cat, __COUNTER__ __VA_OPT__(, ) __VA_ARGS__)

#define TEST_CREATE_FAST(name, ...)                                            \
  TEST_CREATE(name, FAST __VA_OPT__(, ) __VA_ARGS__)

#define TEST_CREATE_SLOW(name, ...)                                            \
  TEST_CREATE(name, SLOW __VA_OPT__(, ) __VA_ARGS__)

/// Require a condition to be true, fail the test if false.
#define TEST_REQUIRE(cond)                                                     \
  ::tkoz::srtest::requireCondition(static_cast<bool>(cond),                    \
                                   "check failed: " #cond)

/// Same as TEST_REQUIRE but with a message for extra information.
#define TEST_REQUIRE_MSG(cond, msg)                                            \
  ::tkoz::srtest::requireCondition(static_cast<bool>(cond), (msg))

/// Require expressions to be equal, with default message.
#define TEST_REQUIRE_EQ(a, b)                                                  \
  ::tkoz::srtest::requireCondition(static_cast<bool>((a) == (b)),              \
                                   "check failed: (" #a ") == (" #b ")")

/// Require expressions to be not equal, with default message.
#define TEST_REQUIRE_NE(a, b)                                                  \
  ::tkoz::srtest::requireCondition(static_cast<bool>((a) != (b)),              \
                                   "check failed: (" #a ") != (" #b ")")

/// Require expressions to compare less than, with default message.
#define TEST_REQUIRE_LT(a, b)                                                  \
  ::tkoz::srtest::requireCondition(static_cast<bool>((a) < (b)),               \
                                   "check failed: (" #a ") < (" #b ")")

/// Require expressions to compare less than or equal to, with default message.
#define TEST_REQUIRE_LE(a, b)                                                  \
  ::tkoz::srtest::requireCondition(static_cast<bool>((a) <= (b)),              \
                                   "check failed: (" #a ") <= (" #b ")")

/// Require expressions to compare greater than, with default message.
#define TEST_REQUIRE_GT(a, b)                                                  \
  ::tkoz::srtest::requireCondition(static_cast<bool>((a) > (b)),               \
                                   "check failed: (" #a ") > (" #b ")")

/// Require expressions to compare greater than or equal to, with default
/// message.
#define TEST_REQUIRE_GE(a, b)                                                  \
  ::tkoz::srtest::requireCondition(static_cast<bool>((a) >= (b)),              \
                                   "check failed: (" #a ") >= (" #b ")")

/// Require expression to not throw an exception.
#define TEST_REQUIRE_NOTHROW(expr)                                             \
  ::tkoz::srtest::requireNoThrow([&]() { static_cast<void>(expr); }, #expr)

/// Require expression to not throw an exception, with custom message.
#define TEST_REQUIRE_NOTHROW_MSG(expr, msg)                                    \
  ::tkoz::srtest::requireNoThrow([&]() { static_cast<void>(expr); }, "", (msg))

/// Require expression to throw specified exception type.
#define TEST_REQUIRE_THROW(expr, type)                                         \
  ::tkoz::srtest::requireThrowExc<type>([&]() { static_cast<void>(expr); },    \
                                        #expr)

/// Require expression to throw specified exception type, with custom message.
#define TEST_REQUIRE_THROW_MSG(expr, type, msg)                                \
  ::tkoz::srtest::requireThrowExc<type>([&]() { static_cast<void>(expr); },    \
                                        "", (msg))

/// Require expression to throw any exception.
#define TEST_REQUIRE_THROW_ANY(expr)                                           \
  ::tkoz::srtest::requireThrowAny([&]() { static_cast<void>(expr); }, #expr)

/// Require expression to throw any exception, with custom message.
#define TEST_REQUIRE_THROW_ANY_MSG(expr, msg)                                  \
  ::tkoz::srtest::requireThrowAny([&]() { static_cast<void>(expr); }, "", (msg))

/// Require 2 floating point numbers to be nearly equal (absolute error).
/// Note: absolute error equal to the given bound is allowed.
#define TEST_REQUIRE_CLOSE_ABS(actual, expected, tolerance)                    \
  ::tkoz::srtest::requireCloseAbs((actual), (expected), (tolerance), #actual,  \
                                  #expected, #tolerance)

/// Require \c actual to be near \c expected with provided relative tolerance.
/// The divisor used is \c expected which is asymmetric.
#define TEST_REQUIRE_CLOSE_REL_EXP(actual, expected, tolerance)                \
  ::tkoz::srtest::requireCloseRel<::tkoz::srtest::RelErrDiv::cExp>(            \
      (actual), (expected), (tolerance), #actual, #expected, #tolerance)

/// Require \c actual to be near \c expected with provided relative tolerance.
/// The divisor used is the larger of \c actual and \c expected which is
/// symmetric and the recommended standard for comparing with relativte error.
#define TEST_REQUIRE_CLOSE_REL_MAX(actual, expected, tolerance)                \
  ::tkoz::srtest::requireCloseRel<::tkoz::srtest::RelErrDiv::cMax>(            \
      (actual), (expected), (tolerance), #actual, #expected, #tolerance)

/// Require \c actual to be near \c expected with provided relative tolerance.
/// The divisor used is the average of \c actual and \c expected which is not as
/// standard for a recommended way to compare with relative error.
#define TEST_REQUIRE_CLOSE_REL_AVG(actual, expected, tolerance)                \
  ::tkoz::srtest::requireCloseRel<::tkoz::srtest::RelErrDiv::cAvg>(            \
      (actual), (expected), (tolerance), #actual, #expected, #tolerance)

/// Require \c actual to be near \c expected with provided relative tolerance.
/// This uses the larger of the 2 as the divisor, the recommended standard.
#define TKOZ_REQUIRE_CLOSE_REL(actual, expected, tolerance)                    \
  TEST_REQUIRE_CLOSE_REL_MAX(actual, expected, tolerance)

/// Require \c actual to be near \c expected with provided ULP tolerance.
/// Note: ULP error equal to the given bound is allowed.
#define TEST_REQUIRE_CLOSE_ULP(actual, expected, tolerance)                    \
  ::tkoz::srtest::requireCloseUlp((actual), (expected), (tolerance), #actual,  \
                                  #expected, #tolerance)

/// Require \c actual to be near \c expected with provided absolute and relative
/// tolerances. This is the same method used by \c math.isclose in Python.
/// The Python default is 1e-9 relative tolerance and 0 absolute tolerance which
/// only makes sense for double precision and ignores checking absolute error.
/// Specify a tolerance of 0 to ignore checking an error type.
#define TEST_REQUIRE_CLOSE(actual, expected, relTol, absTol)                   \
  ::tkoz::srtest::requireClose((actual), (expected), (relTol), (absTol),       \
                               #actual, #expected, #relTol, #absTol)

/// Require \c actual to be near \c expected with provided tolerance. This is
/// simplified to use a single tolerance. It behaves like relative error when
/// an input magnitude is above 1, otherwise it behaves like absolute error.
/// This is equivalent to calling \c TEST_REQUIRE_CLOSE with \c tolerance for
/// both \c relTol and \c absTol .
#define TEST_REQUIRE_NEAR(actual, expected, tolerance)                         \
  ::tkoz::srtest::requireNear((actual), (expected), (tolerance), #actual,      \
                              #expected, #tolerance)

/// Cause a test failure unconditionally with the provided message.
#define TEST_FAILURE(msg)                                                      \
  ::tkoz::srtest::throwFailure((msg), ::tkoz::srtest::sourceLocation())

/// Print a message for test output always.
#define TEST_MESSAGE_ALWAYS(msg) ::tkoz::srtest::addMessage((msg), false)

/// Print a message for test output in the case of failure only.
#define TEST_MESSAGE_FAILURE(msg) ::tkoz::srtest::addMessage((msg), true)

////////////////////////////////////////////////////////////////////////////////
// End of the public interface for the test library.
////////////////////////////////////////////////////////////////////////////////
