#pragma once

#include <format>
#include <functional>
#include <source_location>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace tkoz::srtest {

using TestFunction = std::function<void()>;

/// \brief The data associated with a single test.
struct TestInstance {
  TestInstance() = delete;
  TestInstance(TestFunction func, std::string name)
      : func(std::move(func)), name(std::move(name)) {}
  TestInstance(const TestInstance &) = default;
  TestInstance(TestInstance &&) = default;

  /// Call the test function.
  void operator()() const { func(); }

  /// Function object to run the test
  const TestFunction func;

  /// Name/identifier for the test
  const std::string name;
};

/// \brief The registry storing all statically registered tests.
class TestRegistry {
private:
  std::vector<TestInstance> mAllTests;
  TestRegistry() = default;
  TestRegistry(const TestRegistry &) = delete;
  TestRegistry(TestRegistry &&) = delete;

public:
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

  /// Add a test to the registry.
  /// \param func Function object for running the test
  /// \param name Name/identifier for the test
  inline void addTest(TestFunction func, std::string name) {
    mAllTests.emplace_back(std::move(func), std::move(name));
  }
};

/// Register a test with a function call rather than a static object.
/// For convenience, usually you would use the TEST_CREATE macro.
/// \param name A string identifier for the test.
/// \param func A function object used to run the test.
inline void registerTest(std::string name, std::function<void()> func) {
  TestRegistry::instance().addTest(std::move(func), std::move(name));
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
  TestRegistrar(TestFunction func, std::string name) {
    TestRegistry::instance().addTest(std::move(func), std::move(name));
  }
};

/// \brief Error type thrown by test macros.
class TestFailure : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/// Require a condition to be true, fails the test if false.
/// \param condition A boolean testable
/// \param message An optional message for additional information
/// \param srcLoc Source location object (usually just use the default)
inline void
require(bool condition, std::string message = "",
        std::source_location srcLoc = std::source_location::current()) {
  if (!condition) [[unlikely]] {
    if (message.empty()) {
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

/// Create a test with the provided name (not quoted) and a curly brack {}
/// block following for the function definition.
/// Usage: TEST_CREATE(testName) { ... test code ... }
#define TEST_CREATE(name)                                                      \
  void _tkoz_srtest_func_##name();                                             \
  static ::tkoz::srtest::TestRegistrar _tkoz_srtest_sr_##name(                 \
      _tkoz_srtest_func_##name, #name);                                        \
  void _tkoz_srtest_func_##name()

/// Require a condition to be true, fail the test if false.
#define TEST_REQUIRE(cond) ::tkoz::srtest::require(static_cast<bool>(cond))

/// Same as TEST_REQUIRE but with a message for extra information.
#define TEST_REQUIRE_MSG(cond, msg)                                            \
  ::tkoz::srtest::require(static_cast<bool>(cond), msg)

/// The main function for the test runner. This macro should be defined before
/// including this file in a single source file for the test runner executable.
/// Most likely, that source file would just contain 2 lines:
/// #define TKOZ_SRTEST_MAIN
/// #include <tkoz/SRTest.hpp>
#ifdef TKOZ_SRTEST_MAIN

int main(int argc, char **argv) {
  std::ignore = argc;
  std::ignore = argv;

  const auto &allTests = ::tkoz::srtest::TestRegistry::instance().allTests();
  std::cout << std::format("Found {} tests", allTests.size()) << std::endl;

  for (const auto &test : allTests) {
    std::cout << std::format("Running test: {}", test.name) << std::endl;
    try {
      test();
    } catch (const std::exception &exc) {
      std::cout << exc.what() << std::endl;
      return 1;
    } catch (...) {
      std::cout << "Test failure: unknown exception type" << std::endl;
      return 1;
    }
  }

  std::cout << "Done" << std::endl;
  return 0;
}

#endif
