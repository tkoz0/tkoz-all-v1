/// SRTest - statically registered test library
///
/// Tests are defined along library code and get registered to an executable
/// with static linking.

#pragma once

#include <cstdint>
#include <format>
#include <map>
#include <ranges>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include <cxxabi.h> // Current exception info on GCC/Clang

namespace tkoz::srtest {

using TestFunction = void (*)();

/// \brief Test category enum.
enum class TestCategory : std::uint8_t { FAST, SLOW };

/// Convert a test category to a string
[[maybe_unused]] static inline auto
testCategoryString(TestCategory cat) noexcept -> std::string {
  switch (cat) {
  case TestCategory::FAST:
    return "FAST";
  case TestCategory::SLOW:
    return "SLOW";
  }
  // Make sure all enum values are handled. Compiler should warn if a new one
  // is added but we forget about it.
  std::unreachable();
}

/// \brief The data associated with a single test.
struct TestInstance {
  TestInstance() = delete;
  TestInstance(TestFunction func, std::string name, std::string file,
               std::size_t line, TestCategory cat)
      : func(func), name(std::move(name)), file(std::move(file)), line(line),
        cat(cat) {}
  TestInstance(const TestInstance &) = default;
  TestInstance(TestInstance &&) = default;

  /// Call the test function.
  void run() const { func(); }

  /// Function object to run the test.
  const TestFunction func;

  /// Name/identifier for the test.
  const std::string name;

  /// File containing the test
  const std::string file;

  /// Line where test is defined
  const std::size_t line;

  /// Test category
  const TestCategory cat;
};

/// \brief The registry storing all statically registered tests.
class TestRegistry {
private:
  /// All tests that have been registered.
  std::vector<TestInstance> mAllTests;

  /// Tests grouped by file, keeps track of indexes in mAllTests
  /// Using \a std::map to keep tests ordered by files.
  std::map<std::string, std::vector<std::size_t>> mPerFileTests;

public:
  TestRegistry() = default;
  TestRegistry(const TestRegistry &) = delete;
  TestRegistry(TestRegistry &&) = delete;
  TestRegistry &operator=(const TestRegistry &) = delete;
  TestRegistry &operator=(TestRegistry &&) = delete;

  /// \return The singleton instance keeping track of available tests.
  [[nodiscard]] inline static auto instance() noexcept -> TestRegistry & {
    static TestRegistry testRegistry;
    return testRegistry;
  }

  /// \return All tests contained in the registry.
  [[nodiscard]] inline auto allTests() const noexcept
      -> std::vector<TestInstance> const & {
    return mAllTests;
  }

  /// Access a view listing all files containing registered tests. The view is
  /// invalidated if the registry is modified.
  /// \return All files with tests.
  [[nodiscard]] inline auto allFilesView() const noexcept {
    return std::views::keys(mPerFileTests);
  }

  /// \return Number of tests in a file.
  /// \throw std::out_of_range If the file is not in the registry.
  [[nodiscard]] inline auto
  numTestsInFile(const std::string &file) const noexcept -> std::size_t {
    return mPerFileTests.at(file).size();
  }

  /// Access a view of all test objects in a file. The view is invalidated if
  /// the registry is modified.
  /// \return All tests in a file.
  /// \throw std::out_of_range If the file is not in the registry.
  [[nodiscard]] inline auto
  testsInFileView(std::string const &file) const noexcept {
    return mPerFileTests.at(file) |
           std::views::transform(
               [this](std::size_t index) -> const TestInstance & {
                 return mAllTests[index];
               });
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
    mPerFileTests[file].push_back(mAllTests.size());
    mAllTests.emplace_back(func, std::move(name), std::move(file), line, cat);
  }
};

/// Register a test with a function call rather than a static object.
/// For convenience, usually you would use the TEST_CREATE macro.
/// \param name A string identifier for the test.
/// \param func A function object used to run the test.
inline void registerTest(std::string name, TestFunction func, std::string file,
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
    TestRegistry::instance().addTest(func, std::move(name), std::move(file),
                                     line, cat);
  }
};

/// \brief Error type thrown by test macros.
class TestFailure : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/// Helper to ensure the std::source_location is correctly provided.
/// By using \a consteval, we require the default usage.
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

/// Require a condition to be true, fails the test if false.
/// \param condition A boolean testable.
/// \param falseMsg Message to use for the exception.
/// \a condition is false.
/// \param srcLoc Source location object (use default value).
/// \throw TestFailure If \a condition evaluates to false.
/// \note This function may not work as nicely with compound boolean testables
/// like (a >= 0 && a < n) because it only considers the final value.
/// \note A fundamental limitation (before reflection) is that purely macro free
/// libraries cannot turn the expression into a string, so macros are still very
/// helpful for unit test library design.
inline void requireCondition(bool condition, std::string_view falseMsg,
                             std::source_location srcLoc = sourceLocation()) {
  if (!condition) [[unlikely]] {
    throwFailure(falseMsg, srcLoc);
  }
}

/// Helper to demangle names from the GCC/Clang ABI for current exception.
/// If not possible to get type information, returns \a "unknown type".
/// \param type The \a std::type_info object from
/// \a abi::__cxa_current_exception_type
/// \return C string for the demangled name attempt.
[[nodiscard]] inline auto excName(std::type_info const *type) -> std::string {
  if (!type) {
    return "unknown type";
  }
  int status = -1;
  char *name = abi::__cxa_demangle(type->name(), nullptr, nullptr, &status);
  if (status == 0 && name) {
    std::string result = name;
    std::free(name);
    return result;
  } else {
    return type->name();
  }
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
                          expr, excName(&typeid(exc)), typeid(ExcTypeT).name(),
                          exc.what())
            : custMsg,
        srcLoc);
  } catch (...) {
    std::type_info const *const type = abi::__cxa_current_exception_type();
    throwFailure(custMsg.empty()
                     ? std::format("{} threw {}, expected {}", expr,
                                   excName(type), excName(&typeid(ExcTypeT)))
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
                  expr, excName(&typeid(exc)), exc.what())
            : custMsg,
        srcLoc);
  } catch (...) {
    std::type_info const *const type = abi::__cxa_current_exception_type();
    throwFailure(custMsg.empty()
                     ? std::format("{} threw {}, expected no exception", expr,
                                   excName(type))
                     : custMsg,
                 srcLoc);
  }
}

/// Absolute difference between 2 floating point numbers. Note that the
/// arguments need to be the same type and it is setup to give a useful message
/// if they are not.
/// \param actual The actual computed value.
/// \param expected The expected value.
/// \tparam T Type of the first argument.
/// \tparam U Type of the second argument.
/// \return The absolute difference.
template <typename T, typename U>
  requires std::is_same_v<T, U>
inline T fpErrAbs(T actual, U expected) {
  return std::abs(actual - expected);
}

/// Relative difference between 2 floating point numbers. Note that the
/// arguments need to be the same type and it is setup to give a useful message
/// if they are not.
/// \param actual The actual computed value.
/// \param expected The expected value.
/// \tparam T Type of the first argument.
/// \tparam U Type of the second argument.
/// \return The relative difference.
template <typename T, typename U>
  requires std::is_same_v<T, U>
inline T fpErrRel(T actual, U expected) {
  return std::abs(actual - expected) / expected;
}

} // namespace tkoz::srtest

/// Helper macros to expand macros properly.
#define TKOZ_SRTEST_INTERNAL_CONCAT_4_IMPL(a, b, c, d) a##b##c##d
#define TKOZ_SRTEST_INTERNAL_CONCAT_4(a, b, c, d)                              \
  TKOZ_SRTEST_INTERNAL_CONCAT_4_IMPL(a, b, c, d)

// #define TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT2(a, b) a##b
// #define TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT(a, b)
//   TKOZ_SRTEST_INTERNAL_TEST_NAME_CONCAT2(a, b)

#define TEST_CREATE_INTERNAL(name, cat, counter)                               \
  static void TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_test__, name, __,     \
                                            counter)();                        \
  static ::tkoz::srtest::TestRegistrar TKOZ_SRTEST_INTERNAL_CONCAT_4(          \
      _tkoz_srtest_reg__, name, __, counter)(                                  \
      TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_test__, name, __, counter),   \
      #name, __FILE__, __LINE__, cat);                                         \
  static void TKOZ_SRTEST_INTERNAL_CONCAT_4(_tkoz_srtest_test__, name, __,     \
                                            counter)()

/// Create a test with the provided name (not quoted) and a curly brace {}
/// block following for the function definition.
/// Usage: TEST_CREATE(testName) { ... test code ... }
#define TEST_CREATE(name, cat) TEST_CREATE_INTERNAL(name, cat, __COUNTER__)

#define TEST_CREATE_FAST(name)                                                 \
  TEST_CREATE(name, ::tkoz::srtest::TestCategory::FAST)

#define TEST_CREATE_SLOW(name)                                                 \
  TEST_CREATE(name, ::tkoz::srtest::TestCategory::SLOW)

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
#define TEST_REQUIRE_NEAR_EQ_ABS(actual, expected, error)                      \
  ::tkoz::srtest::requireCondition(                                            \
      ::tkoz::srtest::fpErrAbs((actual), (expected)) < (error),                \
      ::std::format("expected {} to be near {} with absolute error {} "        \
                    "but found absolute error {}",                             \
                    #actual, #expected, #error,                                \
                    ::tkoz::srtest::fpErrAbs((actual), (expected))))

/// Require 2 floating point numbers to be nearly equal (relative error).
#define TEST_REQUIRE_NEAR_EQ_REL(actual, expected, error)                      \
  ::tkoz::srtest::requireCondition(                                            \
      ::tkoz::srtest::fpErrRel((actual), (expected)) < (error),                \
      ::std::format("expected {} to be near {} with relative error {} "        \
                    "but found relative error {}",                             \
                    #actual, #expected, #error,                                \
                    ::tkoz::srtest::fpErrRel((actual), (expected))))

/// The main function for the test runner. This macro should be defined before
/// including this file in a single source file for the test runner executable.
/// Most likely, that source file would just contain 2 lines:
/// #define TKOZ_SRTEST_MAIN
/// #include <tkoz/SRTest.hpp>

#ifdef TKOZ_SRTEST_MAIN

#include <exception>
#include <format>
#include <iostream>
#include <utility>
#include <vector>

int main(int argc, char **argv) {
  std::ignore = argc;
  std::ignore = argv;

  auto const &registry = tkoz::srtest::TestRegistry::instance();

  auto const &allTests = registry.allTests();
  std::cout << std::format("Found {} registered tests", allTests.size())
            << std::endl;

  for (const auto &file : registry.allFilesView()) {
    std::cout << std::format("Found {} tests in file: {}",
                             registry.numTestsInFile(file), file)
              << std::endl;
    for (const auto &test : registry.testsInFileView(file)) {
      std::cout << std::format("Running test: {} ({}, line {})", test.name,
                               tkoz::srtest::testCategoryString(test.cat),
                               test.line)
                << std::endl;
      try {
        test.run();
      } catch (tkoz::srtest::TestFailure const &exc) {
        std::cout << "Test failure: " << exc.what() << std::endl;
        return 1;
      } catch (std::exception const &exc) {
        std::cout << std::format("Test failure ({}): ",
                                 tkoz::srtest::excName(&typeid(exc)))
                  << exc.what() << std::endl;
        return 1;
      } catch (...) {
        std::type_info const *const type = abi::__cxa_current_exception_type();
        std::cout << std::format("Test failure ({})",
                                 tkoz::srtest::excName(type))
                  << std::endl;
        return 1;
      }
    }
  }

  std::cout << "Done" << std::endl;
  return 0;
}

#endif // TKOZ_SRTEST_MAIN
