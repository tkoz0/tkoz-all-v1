/// SRTest - statically registered test library
///
/// Tests are defined along library code and get registered to an executable
/// with static linking.

#pragma once

#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <ranges>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace tkoz::srtest {

using TestFunction = std::function<void()>;

/// \brief Test category enum.
enum class TestCategory : std::uint8_t { FAST, SLOW };

/// Convert a test category to a string
[[maybe_unused]] static std::string testCategoryString(TestCategory cat) {
  switch (cat) {
  case TestCategory::FAST:
    return "FAST";
  case TestCategory::SLOW:
    return "SLOW";
  default:
    throw std::runtime_error("Unhandled test category");
  }
}

/// \brief The data associated with a single test.
struct TestInstance {
  TestInstance() = delete;
  TestInstance(TestFunction func, std::string name, std::string file,
               std::size_t line, TestCategory cat)
      : func(std::move(func)), name(std::move(name)), file(std::move(file)),
        line(line), cat(cat) {}
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
  TestCategory cat;
};

/// \brief The registry storing all statically registered tests.
class TestRegistry {
private:
  /// All tests that have been registered.
  std::vector<TestInstance> mAllTests;

  /// Tests grouped by file, keeps track of indexes in mAllTests
  std::map<std::string, std::vector<std::size_t>> mPerFileTests;

public:
  TestRegistry() = default;
  TestRegistry(const TestRegistry &) = delete;
  TestRegistry(TestRegistry &&) = delete;
  TestRegistry &operator=(const TestRegistry &) = delete;
  TestRegistry &operator=(TestRegistry &&) = delete;

  /// \return The singleton instance keeping track of available tests.
  [[nodiscard]] inline static TestRegistry &instance() noexcept {
    static TestRegistry testRegistry;
    return testRegistry;
  }

  /// \return All tests contained in the registry.
  [[nodiscard]] inline const std::vector<TestInstance> &
  allTests() const noexcept {
    return mAllTests;
  }

  /// \return All files with tests.
  [[nodiscard]] inline auto allFiles() const noexcept {
    return std::views::keys(mPerFileTests);
  }

  /// \return Number of tests in a file.
  [[nodiscard]] inline std::size_t
  numTestsInFile(const std::string &file) const noexcept {
    return mPerFileTests.at(file).size();
  }

  /// \return All tests in a file.
  [[nodiscard]] inline auto
  testsInFile(const std::string &file) const noexcept {
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
  [[nodiscard]] inline std::size_t size() const noexcept {
    return mAllTests.size();
  }

  /// \return True if there are no registered tests.
  [[nodiscard]] inline bool empty() const noexcept { return mAllTests.empty(); }

  /// Add a test to the registry.
  /// \param func Function object for running the test
  /// \param name Name/identifier for the test
  inline void addTest(TestFunction func, std::string name, std::string file,
                      std::size_t line, TestCategory cat) {
    mPerFileTests[file].push_back(mAllTests.size());
    mAllTests.emplace_back(std::move(func), std::move(name), std::move(file),
                           line, cat);
  }
};

/// Register a test with a function call rather than a static object.
/// For convenience, usually you would use the TEST_CREATE macro.
/// \param name A string identifier for the test.
/// \param func A function object used to run the test.
inline void registerTest(std::string name, std::function<void()> func,
                         std::string file, std::size_t line, TestCategory cat) {
  TestRegistry::instance().addTest(std::move(func), std::move(name),
                                   std::move(file), line, cat);
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
    TestRegistry::instance().addTest(std::move(func), std::move(name),
                                     std::move(file), line, cat);
  }
};

/// \brief Error type thrown by test macros.
class TestFailure : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/// Require a condition to be true, fails the test if false.
/// \param condition A boolean testable.
/// \param message An optional message for additional information.
/// \param srcLoc Source location object (usually just use the default).
/// \throw TestFailure If \a condition evaluates to false.
inline void
require(bool condition, std::string_view message = "",
        std::source_location srcLoc = std::source_location::current()) {
  if (!condition) [[unlikely]] {
    if (message.empty()) [[unlikely]] {
      throw TestFailure(std::format("Test failure at {}:{}", srcLoc.file_name(),
                                    srcLoc.line()));
    } else {
      throw TestFailure(std::format("Test failure at {}:{}: {}",
                                    srcLoc.file_name(), srcLoc.line(),
                                    message));
    }
  }
}

} // namespace tkoz::srtest

/// Create a test with the provided name (not quoted) and a curly brace {}
/// block following for the function definition.
/// Usage: TEST_CREATE(testName) { ... test code ... }
#define TEST_CREATE(name, cat)                                                 \
  static void _tkoz_srtest_func_##name();                                      \
  static ::tkoz::srtest::TestRegistrar _tkoz_srtest_reg_##name(                \
      _tkoz_srtest_func_##name, #name, __FILE__, __LINE__, cat);               \
  static void _tkoz_srtest_func_##name()

#define TEST_CREATE_FAST(name)                                                 \
  TEST_CREATE(name, ::tkoz::srtest::TestCategory::FAST)

#define TEST_CREATE_SLOW(name)                                                 \
  TEST_CREATE(name, ::tkoz::srtest::TestCategory::SLOW)

/// Require a condition to be true, fail the test if false.
#define TEST_REQUIRE(cond)                                                     \
  ::tkoz::srtest::require(static_cast<bool>(cond),                             \
                          ::std::format("{} evaluated to false", #cond))

/// Same as TEST_REQUIRE but with a message for extra information.
#define TEST_REQUIRE_MSG(cond, msg)                                            \
  ::tkoz::srtest::require(static_cast<bool>(cond), msg)

/// Require expressions to be equal, with default message.
#define TEST_REQUIRE_EQ(a, b)                                                  \
  ::tkoz::srtest::require(static_cast<bool>((a) == (b)),                       \
                          ::std::format("{} is not equal to {}", #a, #b))

/// Require expressions to be not equal, with default message.
#define TEST_REQUIRE_NE(a, b)                                                  \
  ::tkoz::srtest::require(static_cast<bool>((a) != (b)),                       \
                          ::std::format("{} is equal to {}", #a, #b))

/// Require expressions to compare less than, with default message.
#define TEST_REQUIRE_LT(a, b)                                                  \
  ::tkoz::srtest::require(static_cast<bool>((a) < (b)),                        \
                          ::std::format("{} is not less than {}", #a, #b))

/// Require expressions to compare less than or equal to, with default message.
#define TEST_REQUIRE_LE(a, b)                                                  \
  ::tkoz::srtest::require(                                                     \
      static_cast<bool>((a) <= (b)),                                           \
      ::std::format("{} is not less than or equal to {}", #a, #b))

/// Require expressions to compare greater than, with default message.
#define TEST_REQUIRE_GT(a, b)                                                  \
  ::tkoz::srtest::require(static_cast<bool>((a) > (b)),                        \
                          ::std::format("{} is not greater than {}", #a, #b))

/// Require expressions to compare greater than or equal to, with default
/// message.
#define TEST_REQUIRE_GE(a, b)                                                  \
  ::tkoz::srtest::require(                                                     \
      static_cast<bool>((a) >= (b)),                                           \
      ::std::format("{} is not greater than or equal to {}", #a, #b))

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
  ::std::ignore = argc;
  ::std::ignore = argv;

  const auto &registry = ::tkoz::srtest::TestRegistry::instance();

  const auto &allTests = registry.allTests();
  ::std::cout << ::std::format("Found {} registered tests", allTests.size())
              << ::std::endl;

  for (const auto &file : registry.allFiles()) {
    ::std::cout << ::std::format("Found {} tests in file: {}",
                                 registry.numTestsInFile(file), file)
                << ::std::endl;
    for (const auto &test : registry.testsInFile(file)) {
      ::std::cout << ::std::format("Running test: {} ({}, line {})", test.name,
                                   ::tkoz::srtest::testCategoryString(test.cat),
                                   test.line)
                  << ::std::endl;
      try {
        test.run();
      } catch (const ::std::exception &exc) {
        ::std::cout << exc.what() << ::std::endl;
        return 1;
      } catch (...) {
        ::std::cout << "Test failure: unknown exception type" << ::std::endl;
        return 1;
      }
    }
  }

  ::std::cout << "Done" << ::std::endl;
  return 0;
}

#endif // TKOZ_SRTEST_MAIN
