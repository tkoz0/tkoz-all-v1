
/// The main function for the test runner.
/// Simply include it once for a test runner executable.
/// Most likely, that source file would just contain a single line:
/// #include <tkoz/SRTestMain.hpp>

#include "SRTest.hpp"

#include <chrono>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

namespace tkoz::srtest {

// Formatting

static constexpr const char *FMT_RESET = "\033[0m";
static constexpr const char *FMT_BOLD = "\033[1m";
static constexpr const char *FMT_FAINT = "\033[2m";
static constexpr const char *FMT_ITALIC = "\033[3m";
static constexpr const char *FMT_UNDERLINE = "\033[4m";
static constexpr const char *FMT_BLINK_SLOW = "\033[5m";
static constexpr const char *FMT_BLINK_FAST = "\033[6m";
static constexpr const char *FMT_INVERT_ON = "\033[7m";
static constexpr const char *FMT_HIDE_ON = "\033[8m";
static constexpr const char *FMT_CROSS_ON = "\033[9m";

static constexpr const char *FMT_DOUBLE_UNDERLINE = "\033[21m";
static constexpr const char *FMT_NORMAL_INTENSITY = "\033[22m";
static constexpr const char *FMT_NORMAL_STYLE = "\033[23m";
static constexpr const char *FMT_UNDERLINE_OFF = "\033[24m";
static constexpr const char *FMT_BLINK_OFF = "\033[25m";
static constexpr const char *FMT_INVERT_OFF = "\033[27m";
static constexpr const char *FMT_HIDE_OFF = "\033[28m";
static constexpr const char *FMT_CROSS_OFF = "\033[29m";

// Foreground dark

static constexpr const char *FG_D_BLACK = "\033[30m";
static constexpr const char *FG_D_RED = "\033[31m";
static constexpr const char *FG_D_GREEN = "\033[32m";
static constexpr const char *FG_D_YELLOW = "\033[33m";
static constexpr const char *FG_D_BLUE = "\033[34m";
static constexpr const char *FG_D_MAGENTA = "\033[35m";
static constexpr const char *FG_D_CYAN = "\033[36m";
static constexpr const char *FG_D_WHITE = "\033[37m";

// Foreground bright

static constexpr const char *FG_B_BLACK = "\033[90m";
static constexpr const char *FG_B_RED = "\033[91m";
static constexpr const char *FG_B_GREEN = "\033[92m";
static constexpr const char *FG_B_YELLOW = "\033[93m";
static constexpr const char *FG_B_BLUE = "\033[94m";
static constexpr const char *FG_B_MAGENTA = "\033[95m";
static constexpr const char *FG_B_CYAN = "\033[96m";
static constexpr const char *FG_B_WHITE = "\033[97m";

// Background dark

static constexpr const char *BG_D_BLACK = "\033[40m";
static constexpr const char *BG_D_RED = "\033[41m";
static constexpr const char *BG_D_GREEN = "\033[42m";
static constexpr const char *BG_D_YELLOW = "\033[43m";
static constexpr const char *BG_D_BLUE = "\033[44m";
static constexpr const char *BG_D_MAGENTA = "\033[45m";
static constexpr const char *BG_D_CYAN = "\033[46m";
static constexpr const char *BG_D_WHITE = "\033[47m";

// Background bright

static constexpr const char *BG_B_BLACK = "\033[100m";
static constexpr const char *BG_B_RED = "\033[101m";
static constexpr const char *BG_B_GREEN = "\033[102m";
static constexpr const char *BG_B_YELLOW = "\033[103m";
static constexpr const char *BG_B_BLUE = "\033[104m";
static constexpr const char *BG_B_MAGENTA = "\033[105m";
static constexpr const char *BG_B_CYAN = "\033[106m";
static constexpr const char *BG_B_WHITE = "\033[107m";

/// Command parser. Pass \c argc and \c argv to its constructor.
/// If there is a problem parsing or the help message should be displayed,
/// it handles that and calls \c std::exit . Otherwise, its options are
/// available for use in \c main()
///
/// TODO
/// - filter (-f/--filter ?)
/// - fail immediately or keep running tests (-c/--continue ?)
/// - verbose (-v/--verbose)
/// - quiet (-q/--quiet)
/// - repeat tests (-r/--repeat N)
/// - repeat single test or repeat sequence of selected tests
/// - random test order
/// - timeout per test or overall
/// - output results (text/json?)
/// - colored output control
/// - test type/tag (fast/slow currently)
/// - list tags
/// - list selected tests (-l/--list)
/// - list all tests (maybe --list --all)
/// - list all files
/// - select all tests (--all)
/// - dry run to show what would run and order (-d/--dry-run)
/// - threads for parallelism
class CmdArgs {
private:
  // Program name
  std::string mProgramName = "test-runner";
  // Specified test paths
  std::vector<std::string> mPaths;
  // Failure message if something went wrong
  std::string mFailureMessage;
  // True if no args were specified
  bool mEmpty = false;
  // -h/--help
  bool mHelp = false;

public:
  CmdArgs() = default;

  /// Parse command line arguments and store data to use during test runner
  /// execution.
  inline void parse(int argc, char **argv) {
    if (argc <= 0) {
      mEmpty = true;
      return;
    }
    if (argc == 1) {
      mEmpty = true;
      mProgramName = argv[0];
      return;
    }
    for (int argIndex = 1; argIndex < argc; ++argIndex) {
      std::string arg(argv[argIndex]);
      if (arg.starts_with("--")) {
        // Long option
        if (arg == "--help") {
          mHelp = true;
        } else {
          mFailureMessage = std::format("\"{}\" is not a valid option", arg);
          break;
        }
      } else if (arg.starts_with('-')) {
        // Short option(s)
        if (arg.size() < 2) {
          mFailureMessage = "\"-\" is not a valid option";
          break;
        }
        // Multiple options can be specified like "-abc"
        for (std::size_t optIndex = 1; optIndex < arg.size(); ++optIndex) {
          switch (arg[optIndex]) {
          case 'h':
            mHelp = true;
            break;
          default:
            mFailureMessage =
                std::format("\"-{}\" is not a valid option", arg[optIndex]);
            goto loop_end;
          }
        }
      } else {
        // Positional arguments are test paths
        mPaths.emplace_back(std::move(arg));
      }
    }
  // Goto might not be the greatest idea but if we find a problem then we
  // want to end parsing with the first error message.
  loop_end:
  }

  // Print help message to the ostream with provided program name and optionally
  // exit with code 1.
  void printHelp(std::ostream &stream) const {
    stream << "TKoz SRTest -- Statically registered test library" << std::endl;
    stream << std::format("Usage: {} [-h] [paths...]", mProgramName)
           << std::endl;
    stream << "Test paths are in the form: path/to/dir/sourceFile:testName"
           << std::endl;
    stream << "(start from repository root, do not include .cpp)" << std::endl;
    stream << std::endl;
    stream << "Options:" << std::endl;
    stream << "  -h/--help Print help message and exit" << std::endl;
  }

  /// \return Name of the executable if it can be determined.
  [[nodiscard]] inline auto programName() const noexcept
      -> std::string const & {
    return mProgramName;
  }

  /// \return The paths parsed from the arguments.
  [[nodiscard]] auto paths() const noexcept
      -> std::vector<std::string> const & {
    return mPaths;
  }

  /// \return True if a parsing failure occurred.
  [[nodiscard]] auto failure() const noexcept -> bool {
    return !mFailureMessage.empty();
  }

  /// \return The failure message if a parsing failure occurred, otherwise an
  /// empty \c std::string .
  [[nodiscard]] auto failureMessage() const noexcept -> std::string const & {
    return mFailureMessage;
  }

  /// \return True if there were no arguments.
  [[nodiscard]] auto empty() const noexcept -> bool { return mEmpty; }

  /// \return True if help (-h/--help) was specified.
  [[nodiscard]] auto help() const noexcept -> bool { return mHelp; }
};

/// The stream to write test runner information to. This is intended to be
/// terminal output viewed by developers.
std::ostream &gInfoStream = std::cerr;

/// The command line argument information. At the start of the program, call
/// \c CmdArgs::parse to setup this data.
CmdArgs gCmdArgs;

// Perform a basic filter on all registered tests to determine what to run.
//
// TODO Create some sort of tree of the path structure to make lookup by file
// or directory path more efficient.
auto testsToRun(std::vector<std::string> const &paths)
    -> std::vector<tkoz::srtest::TestCaseInfo *> {
  auto const &registry = tkoz::srtest::TestRegistry::instance();
  std::vector<tkoz::srtest::TestCaseInfo *> result;
  for (std::string const &path : paths) {
    std::size_t sepPos = path.find(':');
    if (sepPos == std::string::npos) {
      // Add all tests in files which begin with the specified path
      for (auto const &test : registry) {
        if (!test->file.starts_with(path)) {
          continue;
        }
        auto iter = test->file.begin() + path.size();
        if (iter == test->file.end() || *iter == '/') {
          result.push_back(test.get());
        }
      }
    } else {
      // Path and test name must match
      std::string testFile = path.substr(0, sepPos);
      std::string testName = path.substr(sepPos + 1);
      for (auto const &test : registry) {
        if (test->file == testFile && test->name == testName) {
          result.push_back(test.get());
        }
      }
    }
  }
  return result;
}

/// Convert a test category to a string
/// \param cat A test category
/// \return A string representation of the category
[[nodiscard]] static inline auto testCategoryString(TestCategory cat) noexcept
    -> std::string const & {
  static constexpr std::string cFast = "FAST";
  static constexpr std::string cSlow = "SLOW";
  switch (cat) {
  case TestCategory::FAST:
    return cFast;
  case TestCategory::SLOW:
    return cSlow;
  }
  // Make sure all enum values are handled. Compiler should warn if a new one
  // is added but we forget about it.
  std::unreachable();
}

/// Test runner output. Write a sequence of values.
/// No separators are included. No new line is included.
template <typename... Ts>
  requires(sizeof...(Ts) > 0)
void infoWrite(Ts &&...values) {
  ((gInfoStream << values), ...);
}

/// Test runner output. Write a sequence of values in color.
/// TODO only write color if allowed by \c gCmdArgs .
template <typename... Ts>
  requires(sizeof...(Ts) > 0)
void infoWriteColored(const char *color, Ts &&...values) {
  gInfoStream << color;
  infoWrite(values...);
  gInfoStream << tkoz::srtest::FMT_RESET;
}

/// Test runner output. Write a sequence of values and a new line.
/// No separators are included.
template <typename... Ts> void infoWriteLine(Ts &&...values) {
  if constexpr (sizeof...(Ts) > 0) {
    infoWrite(values...);
  }
  gInfoStream << std::endl;
}

/// Test runner output. Write a sequence of values in color with a new line.
/// TODO only write color if allowed by \c gCmdArgs .
template <typename... Ts>
void infoWriteColoredLine(const char *color, Ts &&...values) {
  if constexpr (sizeof...(Ts) > 0) {
    infoWriteColored(color, values...);
  }
  gInfoStream << std::endl;
}

} // namespace tkoz::srtest

int main(int argc, char **argv) {
  using namespace tkoz::srtest;
  gCmdArgs.parse(argc, argv);

  if (gCmdArgs.failure()) {
    infoWriteLine("Failed to parse arguments: ", gCmdArgs.failureMessage());
    return 1;
  }
  if (gCmdArgs.empty() || gCmdArgs.help()) {
    gCmdArgs.printHelp(gInfoStream);
    return 1;
  }

  auto const &registry = tkoz::srtest::TestRegistry::instance();
  auto const &allTests = registry.allTests();

  infoWriteColoredLine(tkoz::srtest::FG_B_BLUE,
                       "TKoz SRTest -- Statically Registered Test Library");
  infoWriteLine(std::format("Found {} registered tests", allTests.size()));

  auto selectedTests = testsToRun(gCmdArgs.paths());
  infoWriteLine(std::format("Selected {} tests to run", selectedTests.size()));

  std::size_t numRun = 0;
  std::size_t numSuccess = 0;
  std::size_t numFailed = 0;

  // We should have a monotonicity guarantee for timing performance so it is
  // better to use steady_clock instead of high_resolution_clock.
  using Clock = std::chrono::steady_clock;
  static_assert(Clock::is_steady);
  using TimePoint = decltype(Clock::now());
  using TimeDelta =
      decltype(std::declval<TimePoint>() - std::declval<TimePoint>());
  auto timingsString = [](TimeDelta const &t) -> std::string {
    using namespace std::chrono;
    return std::format("{} / {} / {}", duration_cast<milliseconds>(t),
                       duration_cast<microseconds>(t),
                       duration_cast<nanoseconds>(t));
  };

  for (auto const &test : selectedTests) {
    infoWriteColored(tkoz::srtest::FG_B_BLUE, "Running");
    infoWriteLine(std::format(" {}:{} ({}, line {})", test->file, test->name,
                              tkoz::srtest::testCategoryString(test->cat),
                              test->line));

    bool success = false;
    TimePoint timeStart;
    TimePoint timeFinish;
    try {
      timeStart = Clock::now();
      test->run();
      timeFinish = Clock::now();
      success = true;
    } catch (tkoz::srtest::TestFailure const &exc) {
      infoWriteColored(tkoz::srtest::FG_B_RED, "Test failure");
      infoWriteLine(": ", exc.what());
    } catch (std::exception const &exc) {
      infoWriteColored(tkoz::srtest::FG_B_RED, "Test failure");
      infoWriteLine(" (", tkoz::srtest::typeName(&typeid(exc)),
                    "): ", exc.what());
    } catch (...) {
#if defined(__GNUG__) || defined(__clang__)
      std::type_info const *const type = abi::__cxa_current_exception_type();
#else
      std::type_info const *const type = nullptr;
#endif
      infoWriteColored(tkoz::srtest::FG_B_RED, "Test failure");
      infoWriteLine(" (", tkoz::srtest::typeName(type), ")");
    }
    ++numRun;
    if (success) {
      ++numSuccess;
      infoWriteColored(tkoz::srtest::FG_B_GREEN, "Success");
      infoWriteLine(" (", timingsString(timeFinish - timeStart), ")");
    } else {
      timeFinish = Clock::now();
      ++numFailed;
      infoWriteColored(tkoz::srtest::FG_B_RED, "Failure");
      infoWriteLine(" (", timingsString(timeFinish - timeStart), ")");
      // TODO determine if we should terminate after first failure with cmd args
      return 1;
    }
  }

  // Results
  infoWriteLine(std::format("Completed running {} tests", numRun));
  infoWriteColored(tkoz::srtest::FG_B_GREEN, "Successes");
  infoWriteLine(": ", numSuccess);
  if (numFailed > 0) {
    infoWriteColored(tkoz::srtest::FG_B_RED, "Failures");
  } else {
    infoWrite("Failures");
  }
  infoWriteLine(": ", numFailed);

  // Give a nonzero exit code if any tests failed.
  return numFailed > 0 ? 1 : 0;
}
