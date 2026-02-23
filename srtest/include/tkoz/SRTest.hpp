/// SRTest - statically registered test library
///
/// Tests are defined along library code and get registered to an executable
/// with static linking.
///
/// Do not use anything in the \c tkoz::srtest namespace directly. This is
/// currently internal and subject to change. Write tests using the \c TEST_*
/// macros which are considered the only public interface of the library.

#pragma once

#include <charconv>
#include <compare>
#include <concepts>
#include <cstdint>
#include <format>
#include <memory>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#if defined(__GNUG__) || defined(__clang__)
#include <cxxabi.h> // Current exception info on GCC/Clang
#endif

/// Statically registered test library. The contents of this namespace are not
/// intended to be used by client code and may change. To create tests and test
/// conditions, use the \c TEST_* macros.
///
/// The only thing stopping us from having a unit test library without macros
/// is some nice things like stringifying expressions passed to the condition
/// checking macros.
namespace tkoz::srtest {

using TestFunction = void (*)();

/// \brief Test category enum.
enum class TestCategory : std::uint8_t { FAST, SLOW };

// TODO create a system for test tags in addition to categories.

/// Remove the repo root path from a test filename.
/// \param fullPath The full file path from the \c __FILE__ macro
/// \return The path with the repo root removed.
[[nodiscard]] static inline auto testFilePath(std::string_view fullPath)
    -> std::string {
  static std::string const cRoot = TKOZ_SRTEST_SOURCE_ROOT;
  static std::string const cExt = ".cpp";
  if (!fullPath.starts_with(cRoot)) {
    throw std::runtime_error(
        std::format("test path does not begin with source root: {}", fullPath));
  }
  if (!fullPath.ends_with(cExt)) {
    throw std::runtime_error(
        std::format("test path does not end with \"{}\": {}", cExt, fullPath));
  }
  if (fullPath.size() <= cRoot.size() + 4) {
    throw std::runtime_error(
        std::format("test path appears to be empty: {}", fullPath));
  }
  if (fullPath.contains(':')) {
    throw std::runtime_error(
        std::format("test path contains a color: {}", fullPath));
  }
  return std::string(fullPath.substr(
      cRoot.size(), fullPath.size() - cRoot.size() - cExt.size()));
}

/// \brief The data associated with a single test.
struct TestCaseInfo {
  TestCaseInfo() = delete;
  TestCaseInfo(TestFunction func, std::string name, std::string const &file,
               std::size_t line, TestCategory cat)
      : func(func), name(std::move(name)), file(testFilePath(file)), line(line),
        cat(cat) {}
  TestCaseInfo(TestCaseInfo const &) = default;
  TestCaseInfo(TestCaseInfo &&) = default;

  /// Call the test function.
  void run() const { func(); }

  /// Function object to run the test.
  TestFunction const func;

  /// Name/identifier for the test.
  std::string const name;

  /// File containing the test
  std::string const file;

  /// Line where test is defined
  std::size_t const line;

  /// Test category
  TestCategory const cat;

  /// Define the canonical ordering of tests to be first by file, then by the
  /// order they are defined within a file (line number).
  [[nodiscard]] friend inline auto operator<=>(TestCaseInfo const &a,
                                               TestCaseInfo const &b) noexcept
      -> std::strong_ordering {
    std::strong_ordering fileCmp = a.file <=> b.file;
    if (fileCmp == std::strong_ordering::equal) {
      return a.line <=> b.line;
    } else {
      return fileCmp;
    }
  }
};

/// \brief The registry storing all statically registered tests.
class TestRegistry {
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
  [[nodiscard]] inline static auto instance() noexcept -> TestRegistry & {
    static TestRegistry testRegistry;
    return testRegistry;
  }

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
  /// \param func Function object for running the test
  /// \param name Name/identifier for the test
  inline void addTest(TestFunction func, std::string name, std::string file,
                      std::size_t line, TestCategory cat) {
    mAllTests.push_back(std::make_unique<TestCaseInfo>(
        func, std::move(name), std::move(file), line, cat));
  }
};

/// Register a test with a function call rather than a static object.
/// For convenience, usually you would use the TEST_CREATE macro.
/// \param name A string identifier for the test.
/// \param func A function object used to run the test.
inline void registerTest(TestFunction func, std::string name, std::string file,
                         std::size_t line, TestCategory cat) {
  TestRegistry::instance().addTest(func, std::move(name), std::move(file), line,
                                   cat);
}

/// \brief A class for registering tests. Create a static instance of this
/// class and its constructor registers the test to the TestRegistry.
/// For convenience, usually you would use the TEST_CREATE macro.
class TestRegistrar {
public:
  TestRegistrar() = delete;

  /// This constructor adds a test to the test registry.
  /// \param func The function object to run the test.
  /// \param name A name/identifier for the test.
  TestRegistrar(TestFunction func, std::string name, std::string file,
                std::size_t line, TestCategory cat) {
    registerTest(func, std::move(name), std::move(file), line, cat);
  }
};

/// \brief Error type thrown by test macros.
class TestFailure : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/// Helper to ensure the std::source_location is correctly provided.
/// By using \c consteval , we require the default usage.
[[nodiscard]] inline consteval auto sourceLocation(
    std::source_location srcLoc = std::source_location::current()) noexcept
    -> std::source_location {
  return srcLoc;
}

/// Throw a test failure.
inline void throwFailure(std::string_view msg, std::source_location srcLoc) {
  throw TestFailure(std::format("failure at {}:{}{}{}", srcLoc.file_name(),
                                srcLoc.line(), msg.empty() ? "" : ": ", msg));
}

/// Per thread storage for messages to print at the end.
/// The bool value is false to always print, true to print on failure only.
inline thread_local std::vector<std::pair<bool, std::string>> sTestMessages;

/// Add a message to the thread_local storage for test results.
/// \param msg The message.
/// \param failureOnly True if it should only be printed on test failure.
inline void addMessage(std::string msg, bool failureOnly) {
  sTestMessages.emplace_back(failureOnly, std::move(msg));
}

/// Clears the thread_local message storage for test results.
inline void clearMessages() { sTestMessages.clear(); }

/// Require a condition to be true, fails the test if false.
/// \param condition A boolean testable.
/// \param falseMsg Message to use for the exception.
/// \c condition is false.
/// \param srcLoc Source location object (use default value).
/// \throw TestFailure If \c condition evaluates to false.
/// \note This function may not work as nicely with compound boolean testables
/// like (a >= 0 && a < n) because it only considers the final value.
/// \note A fundamental limitation (before reflection) is that purely macro free
/// libraries cannot turn the expression into a string, so macros are still very
/// helpful for unit test library design.
inline void requireCondition(bool condition, std::string_view falseMsg = "",
                             std::source_location srcLoc = sourceLocation()) {
  if (!condition) [[unlikely]] {
    throwFailure(falseMsg, srcLoc);
  }
}

/// Helper to demangle names from the GCC/Clang ABI for current exception.
/// If not possible to get type information, returns `"unknown type"`.
/// If somehow we compile this with an unsupported compiler, it falls back to
/// simply returning the name without demangling.
/// \param type The \c std::type_info object from
/// \c abi::__cxa_current_exception_type (or nullptr for fallback)
/// \return C string for the demangled name attempt.
[[nodiscard]] inline auto typeName(std::type_info const *type) -> std::string {
  if (!type) {
    return "(unknown type)";
  }
  // Demangle the name with <cxxabi.h> if on GCC or Clang
#if defined(__GNUG__) || defined(__clang__)
  int status = -1;
  char *name = abi::__cxa_demangle(type->name(), nullptr, nullptr, &status);
  if (status == 0 && name) {
    std::string result = name;
    std::free(name);
    return result;
  }
#endif
  return type->name();
}

/// Require an exception of specified type to occur.
/// \param func Function to run to expect an exception from.
/// \param expr Expression string to put into the formatted error messages.
/// \param custMsg Custom message to use instead of the default ones.
/// \param srcLoc Source location object (use default value).
/// \tparam ExcTypeT Exception type expected to be thrown.
/// \tparam FuncT Type of function which is run to test for an exception.
/// \throw TestFailure If an exception of incorrect type is thrown or no
/// exception is thrown.
template <typename ExcTypeT, typename FuncT>
inline void requireThrowExc(FuncT &&func, std::string_view expr,
                            std::string_view custMsg = "",
                            std::source_location srcLoc = sourceLocation()) {
  try {
    std::forward<FuncT>(func)();
  } catch (ExcTypeT const &exc) {
    return; // Successful
  } catch (std::exception const &exc) {
    throwFailure(
        custMsg.empty()
            ? std::format("{} threw {}, expected {}, exception message: {}",
                          expr, typeName(&typeid(exc)), typeid(ExcTypeT).name(),
                          exc.what())
            : custMsg,
        srcLoc);
  } catch (...) {
#if defined(__GNUG__) || defined(__clang__)
    std::type_info const *const type = abi::__cxa_current_exception_type();
#else
    std::type_info const *const type = nullptr;
#endif
    throwFailure(custMsg.empty()
                     ? std::format("{} threw {}, expected {}", expr,
                                   typeName(type), typeName(&typeid(ExcTypeT)))
                     : custMsg,
                 srcLoc);
  }
  throwFailure(custMsg.empty() ? std::format("{} did not throw, expected {}",
                                             expr, typeid(ExcTypeT).name())
                               : custMsg,
               srcLoc);
}

/// Require an exception to be thrown. Type of exception is not checked.
/// \param func Function to run to expect an exception from.
/// \param expr An optional message for additional information.
/// \param custMsg Custom message to use instead of the default ones.
/// \param srcLoc Source location object (use default value).
/// \tparam FuncT Type of function which is run to test for an exception.
/// \throw TestFailure If no except is thrown.
template <typename FuncT>
inline void requireThrowAny(FuncT &&func, std::string_view expr,
                            std::string_view custMsg = "",
                            std::source_location srcLoc = sourceLocation()) {
  try {
    std::forward<FuncT>(func)();
  } catch (...) {
    return; // Successful
  }
  throwFailure(
      custMsg.empty()
          ? std::format("{} did not throw, expected an exception", expr)
          : custMsg,
      srcLoc);
}

/// Require no exception to be thrown.
/// \param func Function to run expecting no exceptions from.
/// \param expr An optional message for additional information.
/// \param custMsg Custom message to use instead of the default ones.
/// \param srcLoc Source location object (use default value).
/// \tparam FuncT Type of function which is run to test for no exception.
/// \throw TestFailure If any exception is thrown.
template <typename FuncT>
inline void requireNothrow(FuncT &&func, std::string_view expr,
                           std::string_view custMsg = "",
                           std::source_location srcLoc = sourceLocation()) {
  try {
    std::forward<FuncT>(func)();
  } catch (std::exception const &exc) {
    throwFailure(
        custMsg.empty()
            ? std::format(
                  "{} threw {}, expected no exception, exception message: {}",
                  expr, typeName(&typeid(exc)), exc.what())
            : custMsg,
        srcLoc);
  } catch (...) {
#if defined(__GNUG__) || defined(__clang__)
    std::type_info const *const type = abi::__cxa_current_exception_type();
#else
    std::type_info const *const type = nullptr;
#endif
    throwFailure(custMsg.empty()
                     ? std::format("{} threw {}, expected no exception", expr,
                                   typeName(type))
                     : custMsg,
                 srcLoc);
  }
}

/// Absolute difference between 2 floating point numbers. For type safety, we
/// require T and U to be the same and have a compiler error if they are not.
/// \param actual The actual computed value.
/// \param expected The expected value.
/// \tparam T Type of the first argument.
/// \tparam U Type of the second argument.
/// \return The absolute difference.
template <std::floating_point T, std::floating_point U>
  requires std::is_same_v<T, U>
inline auto fpErrAbs(T actual, U expected) -> T {
  return std::abs(actual - expected);
}

/// Relative difference between 2 floating point numbers. For type safety, we
/// require T and U to be the same and have a compiler error if they are not.
/// \param actual The actual computed value.
/// \param expected The expected value.
/// \tparam T Type of the first argument.
/// \tparam U Type of the second argument.
/// \return The relative difference.
template <std::floating_point T, std::floating_point U>
  requires std::is_same_v<T, U>
inline auto fpErrRel(T actual, U expected) -> T {
  return std::abs((actual - expected) / expected);
}

/// Check the absolute error against a given error bound. For type safety, we
/// require T, U, and V to be the same and the compiler errors if they are not.
/// \param actual The actual computed value.
/// \param expected The expected value.
/// \param cIncludeEqualT If the error is allowed to be equal to the bound.
/// \param T Type of the first argument.
/// \param U Type of the second argument.
/// \param V Type of the third argument.
/// \return True if the absolute error satisfies the error requirement.
template <bool cIncludeEqualT = true, std::floating_point T,
          std::floating_point U, std::floating_point V>
  requires std::is_same_v<T, U> && std::is_same_v<U, V>
inline auto fpErrAbsCheck(T actual, U expected, V error) -> bool {
  if constexpr (cIncludeEqualT) {
    return fpErrAbs(actual, expected) <= error;
  } else {
    return fpErrAbs(actual, expected) < error;
  }
}

/// Check the relative error against a given error bound. For type safety, we
/// require T, U, and V to be the same and the compiler errors if they are not.
/// \param actual The actual computed value.
/// \param expected The expected value.
/// \param cIncludeEqualT If the error is allowed to be equal to the bound.
/// \param T Type of the first argument.
/// \param U Type of the second argument.
/// \param V Type of the third argument.
/// \return True if the absolute error satisfies the error requirement.
template <bool cIncludeEqualT = true, std::floating_point T,
          std::floating_point U, std::floating_point V>
  requires std::is_same_v<T, U> && std::is_same_v<U, V>
inline auto fpErrRelCheck(T actual, U expected, V error) -> bool {
  if constexpr (cIncludeEqualT) {
    return fpErrRel(actual, expected) <= error;
  } else {
    return fpErrRel(actual, expected) < error;
  }
}

template <typename T>
concept cFpStringSupportedType =
    std::is_same_v<T, float> || std::is_same_v<T, double>;

/// Size of buffer to use for \c std::to_chars result. Some recommendations are
/// to use 32 bytes for float and 64 bytes for double. But the following choices
/// seem to always work.
template <cFpStringSupportedType T>
inline constexpr std::size_t fpStringBufSize = static_cast<std::size_t>(-1);
template <> inline constexpr std::size_t fpStringBufSize<float> = 16;
template <> inline constexpr std::size_t fpStringBufSize<double> = 32;
// template <> inline constexpr std::size_t fpStringBufSize<long double> = 80;

/// String representation of a floating point value. This is meant to be a good
/// format for people to read and guarantee bit exact round trip conversion.
/// \param value The floating point value to convert
/// \tparam T Floating point type
/// \return Decimal string which can be converted back to the exact value.
template <typename T>
inline auto fpString(T value, std::source_location srcLoc = sourceLocation())
    -> std::string {
  char buf[fpStringBufSize<T>];
  auto const [bufEnd, error] =
      std::to_chars(buf, buf + fpStringBufSize<T>, value);
  if (error != std::errc{}) {
    throwFailure("internal failure converting floating point to string",
                 srcLoc);
  }
  return std::string(buf, bufEnd);
}

} // namespace tkoz::srtest

/// Helper macros to expand macros properly.
#define TKOZ_SRTEST_INTERNAL_CONCAT_4_IMPL(a, b, c, d) a##b##c##d
#define TKOZ_SRTEST_INTERNAL_CONCAT_4(a, b, c, d)                              \
  TKOZ_SRTEST_INTERNAL_CONCAT_4_IMPL(a, b, c, d)

// #define TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT2(a, b) a##b
// #define TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT(a, b)
//   TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT2(a, b)

#define TKOZ_SRTEST_INTERNAL_CREATE(name, cat, counter)                        \
  static void TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_testfunc__, name, __, \
                                            counter)();                        \
  struct _tkoz_srtest_names_unique__##name {};                                 \
  [[maybe_unused]] static ::tkoz::srtest::TestRegistrar                        \
      TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_registrar__, name, __,        \
                                    counter)(                                  \
          TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_testfunc__, name, __,     \
                                        counter),                              \
          #name, __FILE__, __LINE__, ::tkoz::srtest::TestCategory::cat);       \
  static void TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_testfunc__, name, __, \
                                            counter)()

////////////////////////////////////////////////////////////////////////////////
// These macros actually define the public interface for the test library.
////////////////////////////////////////////////////////////////////////////////

/// Create a test with the provided name (not quoted) and a curly brace {}
/// block following for the function definition.
/// Usage: TEST_CREATE(testName) { ... test code ... }
#define TEST_CREATE(name, cat)                                                 \
  TKOZ_SRTEST_INTERNAL_CREATE(name, cat, __COUNTER__)

#define TEST_CREATE_FAST(name) TEST_CREATE(name, FAST)

#define TEST_CREATE_SLOW(name) TEST_CREATE(name, SLOW)

/// Require a condition to be true, fail the test if false.
#define TEST_REQUIRE(cond)                                                     \
  ::tkoz::srtest::requireCondition(                                            \
      static_cast<bool>(cond), ::std::format("{} evaluated to false", #cond))

/// Same as TEST_REQUIRE but with a message for extra information.
#define TEST_REQUIRE_MSG(cond, msg)                                            \
  ::tkoz::srtest::requireCondition(static_cast<bool>(cond), msg)

/// Require expressions to be equal, with default message.
#define TEST_REQUIRE_EQ(a, b)                                                  \
  ::tkoz::srtest::requireCondition(                                            \
      static_cast<bool>((a) == (b)),                                           \
      ::std::format("{} is not equal to {}", #a, #b))

/// Require expressions to be not equal, with default message.
#define TEST_REQUIRE_NE(a, b)                                                  \
  ::tkoz::srtest::requireCondition(static_cast<bool>((a) != (b)),              \
                                   ::std::format("{} is equal to {}", #a, #b))

/// Require expressions to compare less than, with default message.
#define TEST_REQUIRE_LT(a, b)                                                  \
  ::tkoz::srtest::requireCondition(                                            \
      static_cast<bool>((a) < (b)),                                            \
      ::std::format("{} is not less than {}", #a, #b))

/// Require expressions to compare less than or equal to, with default message.
#define TEST_REQUIRE_LE(a, b)                                                  \
  ::tkoz::srtest::requireCondition(                                            \
      static_cast<bool>((a) <= (b)),                                           \
      ::std::format("{} is not less than or equal to {}", #a, #b))

/// Require expressions to compare greater than, with default message.
#define TEST_REQUIRE_GT(a, b)                                                  \
  ::tkoz::srtest::requireCondition(                                            \
      static_cast<bool>((a) > (b)),                                            \
      ::std::format("{} is not greater than {}", #a, #b))

/// Require expressions to compare greater than or equal to, with default
/// message.
#define TEST_REQUIRE_GE(a, b)                                                  \
  ::tkoz::srtest::requireCondition(                                            \
      static_cast<bool>((a) >= (b)),                                           \
      ::std::format("{} is not greater than or equal to {}", #a, #b))

/// Require expression to not throw an exception.
#define TEST_REQUIRE_NOTHROW(expr)                                             \
  ::tkoz::srtest::requireNothrow([&]() { static_cast<void>(expr); }, #expr)

/// Require expression to not throw an exception, with custom message.
#define TEST_REQUIRE_NOTHROW_MSG(expr, msg)                                    \
  ::tkoz::srtest::requireNothrow([&]() { static_cast<void>(expr); }, "", msg)

/// Require expression to throw specified exception type.
#define TEST_REQUIRE_THROW(expr, type)                                         \
  ::tkoz::srtest::requireThrowExc<type>([&]() { static_cast<void>(expr); },    \
                                        #expr);

/// Require expression to throw specified exception type, with custom message.
#define TEST_REQUIRE_THROW_MSG(expr, type, msg)                                \
  ::tkoz::srtest::requireThrowExc<type>([&]() { static_cast<void>(expr); },    \
                                        "", msg)

/// Require expression to throw any exception.
#define TEST_REQUIRE_THROW_ANY(expr)                                           \
  ::tkoz::srtest::requireThrowAny([&]() { static_cast<void>(expr); }, #expr);

/// Require expression to throw any exception, with custom message.
#define TEST_REQUIRE_THROW_ANY_MSG(expr, msg)                                  \
  ::tkoz::srtest::requireThrowAny([&]() { static_cast<void>(expr); }, "", msg);

/// Require 2 floating point numbers to be nearly equal (absolute error).
/// Note: absolute error equal to the provided error bound is allowed.
#define TEST_REQUIRE_NEAR_EQ_ABS(actual, expected, error)                      \
  ::tkoz::srtest::requireCondition(                                            \
      ::tkoz::srtest::fpErrAbsCheck((actual), (expected), (error)),            \
      ::std::format("expected {} ({}) to be near {} ({}) "                     \
                    "with absolute error at most {} ({}) "                     \
                    "but found absolute error {}",                             \
                    #actual, ::tkoz::srtest::fpString(actual), #expected,      \
                    ::tkoz::srtest::fpString(expected), #error,                \
                    ::tkoz::srtest::fpString(error),                           \
                    ::tkoz::srtest::fpString(                                  \
                        ::tkoz::srtest::fpErrAbs((actual), (expected)))))

/// Require 2 floating point numbers to be nearly equal (relative error).
/// Note: relative error equal to the provided error bound is allowed.
#define TEST_REQUIRE_NEAR_EQ_REL(actual, expected, error)                      \
  ::tkoz::srtest::requireCondition(                                            \
      ::tkoz::srtest::fpErrRelCheck((actual), (expected), (error)),            \
      ::std::format("expected {} ({}) to be near {} ({}) "                     \
                    "with relative error at most {} ({}) "                     \
                    "but found relative error {}",                             \
                    #actual, ::tkoz::srtest::fpString(actual), #expected,      \
                    ::tkoz::srtest::fpString(expected), #error,                \
                    ::tkoz::srtest::fpString(error),                           \
                    ::tkoz::srtest::fpString(                                  \
                        ::tkoz::srtest::fpErrRel((actual), (expected)))))

/// Cause a test failure unconditionally with the provided message.
#define TEST_FAILURE(msg) TEST_REQUIRE_MSG(0, msg)

/// Print a message for test output always.
#define TEST_MESSAGE_ALWAYS(msg) ::tkoz::srtest::addMessage(msg, false)

/// Print a message for test output in the case of failure only.
#define TEST_MESSAGE_FAILURE(msg) ::tkoz::srtest::addMessage(msg, true)

////////////////////////////////////////////////////////////////////////////////
// End of the public interface for the test library.
////////////////////////////////////////////////////////////////////////////////
