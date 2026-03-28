/// SRTest - statically registered test library
///
/// The main function for the test runner.
/// Simply include it once for a test runner executable.
/// Most likely, that source file would just contain a single line:
/// #include <tkoz/SRTestMain.hpp>

#include "SRTest.hpp"

#include <bit>
#include <bitset>
#include <chrono>
#include <compare>
#include <concepts>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>
#include <optional>
#include <ranges>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <typeinfo>
#include <utility>
#include <vector>

#if defined(__GNUG__) || defined(__clang__)
#include <cxxabi.h> // Current exception info on GCC/Clang
#endif

// Definitions for things in SRTest.hpp
namespace tkoz::srtest {

auto TestTags::tagString(TagEnum aTag) -> std::string const & {
  if (aTag == TAG_COUNT) {
    throw std::invalid_argument(
        "TAG_COUNT is reserved and not a valid test tag");
  }
  static std::vector<std::string> const sStrings = {"FAST", "SLOW"};
  return sStrings.at(static_cast<std::size_t>(aTag));
}

auto TestTags::allTags() const noexcept -> std::vector<TagEnum> {
  return std::ranges::to<std::vector<TagEnum>>(
      std::views::iota(std::size_t{0}, cMaxTags) |
      std::views::filter(
          [this](std::size_t i) -> bool { return mTagBits.test(i); }) |
      std::views::transform(
          [](std::size_t i) -> TagEnum { return static_cast<TagEnum>(i); }));
}

auto testFilePath(std::string_view aFullPath) -> std::string {
  static std::string const cRoot = TKOZ_SRTEST_SOURCE_ROOT;
  static std::string const cExt = TKOZ_SRTEST_SOURCE_EXT;
  if (!aFullPath.starts_with(cRoot)) {
    throw std::runtime_error(std::format(
        "test path does not begin with source root: {}", aFullPath));
  }
  if (!aFullPath.ends_with(cExt)) {
    throw std::runtime_error(
        std::format("test path does not end with \"{}\": {}", cExt, aFullPath));
  }
  if (aFullPath.size() <= cRoot.size() + 4) {
    throw std::runtime_error(
        std::format("test path appears to be empty: {}", aFullPath));
  }
  if (aFullPath.contains(':')) {
    throw std::runtime_error(
        std::format("test path contains a color: {}", aFullPath));
  }
  return std::string(aFullPath.substr(
      cRoot.size(), aFullPath.size() - cRoot.size() - cExt.size()));
}

auto operator<=>(TestCaseInfo const &aLeft, TestCaseInfo const &aRight) noexcept
    -> std::strong_ordering {
  std::strong_ordering const lFileCmp = aLeft.mFile <=> aRight.mFile;
  if (lFileCmp == std::strong_ordering::equal) {
    return aLeft.mLine <=> aRight.mLine;
  } else {
    return lFileCmp;
  }
}

auto TestRegistry::instance() noexcept -> TestRegistry & {
  static TestRegistry sTestRegistry;
  return sTestRegistry;
}

void throwFailure(std::string_view aMessage, std::source_location aSrcLoc) {
  throw TestFailure(std::format("failure at {}:{}{}{}", aSrcLoc.file_name(),
                                aSrcLoc.line(), aMessage.empty() ? "" : ": ",
                                aMessage));
}

// Per thread storage for messages to print at the end.
// The bool value is false to always print, true to print on failure only.
inline thread_local std::vector<std::pair<bool, std::string>> gTestMessages;

void addMessage(std::string aMessage, bool aFailureOnly) {
  gTestMessages.emplace_back(aFailureOnly, std::move(aMessage));
}

void clearMessages() { gTestMessages.clear(); }

void requireCondition(bool aCondition, std::string_view aFalseMsg,
                      std::source_location aSrcLoc) {
  if (!aCondition) [[unlikely]] {
    throwFailure(aFalseMsg, aSrcLoc);
  }
}

auto typeName(std::type_info const *aTypeInfo) -> std::string {
  if (!aTypeInfo) {
    return "(unknown type)";
  }
  // Demangle the name with <cxxabi.h> if on GCC or Clang
#if defined(__GNUG__) || defined(__clang__)
  int lStatus = -1;
  char *const lName =
      abi::__cxa_demangle(aTypeInfo->name(), nullptr, nullptr, &lStatus);
  if (lStatus == 0 && lName) {
    std::string result = lName;
    std::free(lName);
    return result;
  }
#endif
  return aTypeInfo->name();
}

namespace internal {

void requireThrowExcHelper(std::exception_ptr const &aExcPtr,
                           std::string_view aExpr, std::string_view aCustMsg,
                           std::string_view aExpectedTypeName,
                           std::source_location aSrcLoc) {
  if (aExcPtr) {
    try {
      std::rethrow_exception(aExcPtr);
    } catch (std::exception const &lExc) {
      throwFailure(
          aCustMsg.empty()
              ? std::format("{} threw {}, expected {}, exception message: {}",
                            aExpr, typeName(&typeid(lExc)), aExpectedTypeName,
                            lExc.what())
              : aCustMsg,
          aSrcLoc);
    } catch (...) {
#if defined(__GNUG__) || defined(__clang__)
      std::type_info const *const lType = abi::__cxa_current_exception_type();
#else
      std::type_info const *const lType = nullptr;
#endif
      throwFailure(aCustMsg.empty()
                       ? std::format("{} threw {}, expected {}", aExpr,
                                     typeName(lType), aExpectedTypeName)
                       : aCustMsg,
                   aSrcLoc);
    }
  } else {
    throwFailure(aCustMsg.empty() ? std::format("{} did not throw, expected {}",
                                                aExpr, aExpectedTypeName)
                                  : aCustMsg,
                 aSrcLoc);
  }
}

void requireThrowAnyHelper(std::string_view aExpr, std::string_view aCustMsg,
                           std::source_location aSrcLoc) {
  throwFailure(
      aCustMsg.empty()
          ? std::format("{} did not throw, expected an exception", aExpr)
          : aCustMsg,
      aSrcLoc);
}

// Note that this requires a non null exception pointer.
void requireNoThrowHelper(std::exception_ptr const &aExcPtr,
                          std::string_view aExpr, std::string_view aCustMsg,
                          std::source_location aSrcLoc) {
  try {
    std::rethrow_exception(aExcPtr);
  } catch (std::exception const &lExc) {
    throwFailure(
        aCustMsg.empty()
            ? std::format(
                  "{} threw {}, expected no exception, exception message: {}",
                  aExpr, typeName(&typeid(lExc)), lExc.what())
            : aCustMsg,
        aSrcLoc);
  } catch (...) {
#if defined(__GNUG__) || defined(__clang__)
    std::type_info const *const lType = abi::__cxa_current_exception_type();
#else
    std::type_info const *const lType = nullptr;
#endif
    throwFailure(aCustMsg.empty()
                     ? std::format("{} threw {}, expected no exception", aExpr,
                                   typeName(lType))
                     : aCustMsg,
                 aSrcLoc);
  }
}

template <std::floating_point T> inline std::size_t cFpStringBufSize;
template <> inline constexpr std::size_t cFpStringBufSize<float> = 16;
template <> inline constexpr std::size_t cFpStringBufSize<double> = 32;

// String representation of a floating point. Shortest decimal that converts
// back to the original value exactly.
template <std::floating_point T>
  requires(!std::is_same_v<T, long double>)
inline auto fpString(T aValue,
                     std::source_location aSrcLoc = sourceLocation()) {
  char lBuf[cFpStringBufSize<T>];
  auto const [lBufEnd, lError] =
      std::to_chars(lBuf, lBuf + cFpStringBufSize<T>, aValue);
  if (lError != std::errc{}) [[unlikely]] {
    throwFailure("internal failure converting floating point to string",
                 aSrcLoc);
  }
  return std::string(lBuf, lBufEnd);
}

template <typename T>
auto fpErrUlpImpl(T aActual, T aExpected) -> FpBits<T>::Unsigned {
  static_assert(std::is_floating_point_v<T> && !std::is_same_v<T, long double>);
  static_assert(std::numeric_limits<T>::is_iec559,
                "ULP distance assumes IEEE 754 representation");
  using ReturnT = FpBits<T>::Unsigned;
  using BitsT = FpBits<T>::Signed;
  if (std::isnan(aActual) || std::isnan(aExpected)) {
    return std::numeric_limits<ReturnT>::max();
  }
  if (aActual == aExpected) {
    return ReturnT{0}; // This handles infinities and +0 == -0
  }
  BitsT lBitsActual = std::bit_cast<BitsT>(aActual);
  BitsT lBitsExpected = std::bit_cast<BitsT>(aExpected);
  // Positive float values as bits correspond to positive signed integer values
  // Negative float values are ordered backwards, switch negative signed values
  if (lBitsActual < BitsT{0}) {
    lBitsActual = std::numeric_limits<BitsT>::min() - lBitsActual;
  }
  if (lBitsExpected < BitsT{0}) {
    lBitsExpected = std::numeric_limits<BitsT>::min() - lBitsExpected;
  }
  return lBitsActual > lBitsExpected ? static_cast<ReturnT>(lBitsActual) -
                                           static_cast<ReturnT>(lBitsExpected)
                                     : static_cast<ReturnT>(lBitsExpected) -
                                           static_cast<ReturnT>(lBitsActual);
}

template auto fpErrUlpImpl(float, float) -> std::uint32_t;
template auto fpErrUlpImpl(double, double) -> std::uint64_t;

template <bool cIncludeEqualT, std::floating_point T>
void requireCloseAbsImpl(T aActual, T aExpected, T aTolerance,
                         char const *aActualStr, char const *aExpectedStr,
                         char const *aToleranceStr,
                         std::source_location aSrcLoc) {
  T const lError = fpErrAbs(aActual, aExpected);
  bool lResult;
  if constexpr (cIncludeEqualT) {
    lResult = lError <= aTolerance;
  } else {
    lResult = lError < aTolerance;
  }
  if (!lResult) [[unlikely]] {
    throwFailure(std::format("expected {} ({}) to be near {} ({}) "
                             "with absolute error at most {} ({}) "
                             "but found absolute error {}",
                             aActualStr, fpString(aActual), aExpectedStr,
                             fpString(aExpected), aToleranceStr,
                             fpString(aTolerance), fpString(lError)),
                 aSrcLoc);
  }
}

template void requireCloseAbsImpl<true, float>(float, float, float,
                                               char const *, char const *,
                                               char const *,
                                               std::source_location);
template void requireCloseAbsImpl<true, double>(double, double, double,
                                                char const *, char const *,
                                                char const *,
                                                std::source_location);
template void requireCloseAbsImpl<false, float>(float, float, float,
                                                char const *, char const *,
                                                char const *,
                                                std::source_location);
template void requireCloseAbsImpl<false, double>(double, double, double,
                                                 char const *, char const *,
                                                 char const *,
                                                 std::source_location);

template <RelErrDiv cDivT, bool cIncludeEqualT, std::floating_point T>
void requireCloseRelImpl(T aActual, T aExpected, T aTolerance,
                         char const *aActualStr, char const *aExpectedStr,
                         char const *aToleranceStr,
                         std::source_location aSrcLoc) {
  // We avoid division and instead use multiplication to check relative error
  T lLhs = fpErrAbs(aActual, aExpected);
  T lRhs = aTolerance;
  if constexpr (cDivT == RelErrDiv::cExp) {
    lRhs *= aExpected;
  } else if constexpr (cDivT == RelErrDiv::cMax) {
    lRhs *= std::max(std::abs(aActual), std::abs(aExpected));
  } else if constexpr (cDivT == RelErrDiv::cAvg) {
    lLhs *= T{2.0};
    lRhs *= std::abs(aActual) + std::abs(aExpected);
  } else {
    static_assert(false, "unexpected relative error divisor type");
  }
  bool lResult;
  if constexpr (cIncludeEqualT) {
    lResult = lLhs <= lRhs;
  } else {
    lResult = lLhs < lRhs;
  }
  if (!lResult) [[unlikely]] {
    throwFailure(std::format("expected {} ({}) to be near {} ({}) "
                             "with relative error at most {} ({}) "
                             "but found relative error {}",
                             aActualStr, fpString(aActual), aExpectedStr,
                             fpString(aExpected), aToleranceStr,
                             fpString(aTolerance),
                             fpString(fpErrRel<cDivT>(aActual, aExpected))),
                 aSrcLoc);
  }
}

template void requireCloseRelImpl<RelErrDiv::cExp, true, float>(
    float, float, float, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cExp, true, double>(
    double, double, double, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cExp, false, float>(
    float, float, float, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cExp, false, double>(
    double, double, double, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cMax, true, float>(
    float, float, float, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cMax, true, double>(
    double, double, double, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cMax, false, float>(
    float, float, float, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cMax, false, double>(
    double, double, double, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cAvg, true, float>(
    float, float, float, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cAvg, true, double>(
    double, double, double, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cAvg, false, float>(
    float, float, float, char const *, char const *, char const *,
    std::source_location);
template void requireCloseRelImpl<RelErrDiv::cAvg, false, double>(
    double, double, double, char const *, char const *, char const *,
    std::source_location);

template <bool cIncludeEqualT, std::floating_point T, typename U>
void requireCloseUlpImpl(T aActual, T aExpected, U aTolerance,
                         char const *aActualStr, char const *aExpectedStr,
                         char const *aToleranceStr,
                         std::source_location aSrcLoc) {
  static_assert(std::is_same_v<U, decltype(fpErrUlp(aActual, aExpected))>);
  U const lError = fpErrUlp(aActual, aExpected);
  bool lResult;
  if constexpr (cIncludeEqualT) {
    lResult = lError <= aTolerance;
  } else {
    lResult = lError < aTolerance;
  }
  if (!lResult) [[unlikely]] {
    throwFailure(std::format("expected {} ({}) to be near {} ({}) "
                             "with ULP error at most {} ({}) "
                             "but found ULP error {}",
                             aActualStr, fpString(aActual), aExpectedStr,
                             fpString(aExpected), aToleranceStr,
                             std::to_string(aTolerance),
                             std::to_string(lError)),
                 aSrcLoc);
  }
}

template void requireCloseUlpImpl<true, float, std::uint32_t>(
    float, float, std::uint32_t, char const *, char const *, char const *,
    std::source_location);
template void requireCloseUlpImpl<true, double, std::uint64_t>(
    double, double, std::uint64_t, char const *, char const *, char const *,
    std::source_location);
template void requireCloseUlpImpl<false, float, std::uint32_t>(
    float, float, std::uint32_t, char const *, char const *, char const *,
    std::source_location);
template void requireCloseUlpImpl<false, double, std::uint64_t>(
    double, double, std::uint64_t, char const *, char const *, char const *,
    std::source_location);

template <bool cIncludeEqualT, std::floating_point T>
void requireCloseImpl(T aActual, T aExpected, T aRelTol, T aAbsTol,
                      char const *aActualStr, char const *aExpectedStr,
                      char const *aRelTolStr, char const *aAbsTolStr,
                      std::source_location aSrcLoc) {
  T const lRelVal = aRelTol * std::max(std::abs(aActual), std::abs(aExpected));
  bool lResult;
  if constexpr (cIncludeEqualT) {
    lResult = fpErrAbs(aActual, aExpected) <= std::max(lRelVal, aAbsTol);
  } else {
    lResult = fpErrAbs(aActual, aExpected) < std::max(lRelVal, aAbsTol);
  }
  if (!lResult) [[unlikely]] {
    throwFailure(std::format("expected {} ({}) to be close to {} ({}) "
                             "with relative error at most {} ({}) "
                             "or absolute error at most {} ({}) "
                             "but found relative error {} "
                             "and absolute error {}",
                             aActualStr, fpString(aActual), aExpectedStr,
                             fpString(aExpected), aRelTolStr, fpString(aRelTol),
                             aAbsTolStr, fpString(aAbsTol),
                             fpErrRel(aActual, aExpected),
                             fpErrAbs(aActual, aExpected)),
                 aSrcLoc);
  }
}

template void requireCloseImpl<true, float>(float, float, float, float,
                                            char const *, char const *,
                                            char const *, char const *,
                                            std::source_location);
template void requireCloseImpl<true, double>(double, double, double, double,
                                             char const *, char const *,
                                             char const *, char const *,
                                             std::source_location);
template void requireCloseImpl<false, float>(float, float, float, float,
                                             char const *, char const *,
                                             char const *, char const *,
                                             std::source_location);
template void requireCloseImpl<false, double>(double, double, double, double,
                                              char const *, char const *,
                                              char const *, char const *,
                                              std::source_location);

template <bool cIncludeEqualT, std::floating_point T>
void requireNearImpl(T aActual, T aExpected, T aTolerance,
                     char const *aActualStr, char const *aExpectedStr,
                     char const *aToleranceStr, std::source_location aSrcLoc) {
  T const lCompVal =
      aTolerance *
      std::max(T{1.0}, std::max(std::abs(aActual), std::abs(aExpected)));
  bool lResult;
  if constexpr (cIncludeEqualT) {
    lResult = fpErrAbs(aActual, aExpected) <= lCompVal;
  } else {
    lResult = fpErrAbs(aActual, aExpected) < lCompVal;
  }
  if (!lResult) [[unlikely]] {
    throwFailure(std::format("expected {} ({}) to be near {} ({}) "
                             "with tolerance at most {} ({}) "
                             "but found relative error {}"
                             "and absolute error {}",
                             aActualStr, fpString(aActual), aExpectedStr,
                             fpString(aExpected), aToleranceStr,
                             fpString(aTolerance), fpErrRel(aActual, aExpected),
                             fpErrAbs(aActual, aExpected)),
                 aSrcLoc);
  }
}

template void requireNearImpl<true, float>(float, float, float, char const *,
                                           char const *, char const *,
                                           std::source_location);
template void requireNearImpl<true, double>(double, double, double,
                                            char const *, char const *,
                                            char const *, std::source_location);
template void requireNearImpl<false, float>(float, float, float, char const *,
                                            char const *, char const *,
                                            std::source_location);
template void requireNearImpl<false, double>(double, double, double,
                                             char const *, char const *,
                                             char const *,
                                             std::source_location);

} // namespace internal

} // namespace tkoz::srtest
// End of definitions for things in SRTest.hpp

// Definitions for things the main runner uses
namespace tkoz::srtest {

// Formatting

static constexpr const char *cFmtReset = "\033[0m";
static constexpr const char *cFmtBold = "\033[1m";
static constexpr const char *cFmtFaint = "\033[2m";
static constexpr const char *cFmtItalic = "\033[3m";
static constexpr const char *cFmtUnderlineOn = "\033[4m";
static constexpr const char *cFmtBlinkSlow = "\033[5m";
static constexpr const char *cFmtBlinkFast = "\033[6m";
static constexpr const char *cFmtInventOn = "\033[7m";
static constexpr const char *cFmtHideOn = "\033[8m";
static constexpr const char *cFmtCrossOn = "\033[9m";

static constexpr const char *cFmtDoubleUnderline = "\033[21m";
static constexpr const char *cFmtNormalIntensity = "\033[22m";
static constexpr const char *cFmtNormalStyle = "\033[23m";
static constexpr const char *cFmtUnderlineOff = "\033[24m";
static constexpr const char *cFmtBlinkOff = "\033[25m";
static constexpr const char *cFmtInvertOff = "\033[27m";
static constexpr const char *cFmtHideOff = "\033[28m";
static constexpr const char *cFmtCrossOff = "\033[29m";

// Foreground dark

static constexpr const char *cFgDBlack = "\033[30m";
static constexpr const char *cFgDRed = "\033[31m";
static constexpr const char *cFgDGreen = "\033[32m";
static constexpr const char *cFgDYellow = "\033[33m";
static constexpr const char *cFgDBlue = "\033[34m";
static constexpr const char *cFgDMagenta = "\033[35m";
static constexpr const char *cFgDCyan = "\033[36m";
static constexpr const char *cFgDWhite = "\033[37m";

// Foreground bright

static constexpr const char *cFgBBlack = "\033[90m";
static constexpr const char *cFgBRed = "\033[91m";
static constexpr const char *cFgBGreen = "\033[92m";
static constexpr const char *cFgBYellow = "\033[93m";
static constexpr const char *cFgBBlue = "\033[94m";
static constexpr const char *cFgBMagenta = "\033[95m";
static constexpr const char *cFgBCyan = "\033[96m";
static constexpr const char *cFgBWhite = "\033[97m";

// Background dark

static constexpr const char *cBgDBlack = "\033[40m";
static constexpr const char *cBgDRed = "\033[41m";
static constexpr const char *cBgDGreen = "\033[42m";
static constexpr const char *cBgDYellow = "\033[43m";
static constexpr const char *cBgDBlue = "\033[44m";
static constexpr const char *cBgDMagenta = "\033[45m";
static constexpr const char *cBgDCyan = "\033[46m";
static constexpr const char *cBgDWhite = "\033[47m";

// Background bright

static constexpr const char *cBgBBlack = "\033[100m";
static constexpr const char *cBgBRed = "\033[101m";
static constexpr const char *cBgBGreen = "\033[102m";
static constexpr const char *cBgBYellow = "\033[103m";
static constexpr const char *cBgBBlue = "\033[104m";
static constexpr const char *cBgBMagenta = "\033[105m";
static constexpr const char *cBgBCyan = "\033[106m";
static constexpr const char *cBgBWhite = "\033[107m";

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
    for (int lArgIndex = 1; lArgIndex < argc; ++lArgIndex) {
      std::string lArg(argv[lArgIndex]);
      if (lArg.starts_with("--")) {
        // Long option
        if (lArg == "--help") {
          mHelp = true;
        } else {
          mFailureMessage = std::format("\"{}\" is not a valid option", lArg);
          break;
        }
      } else if (lArg.starts_with('-')) {
        // Short option(s)
        if (lArg.size() < 2) {
          mFailureMessage = "\"-\" is not a valid option";
          break;
        }
        // Multiple options can be specified like "-abc"
        for (std::size_t lOptIndex = 1; lOptIndex < lArg.size(); ++lOptIndex) {
          switch (lArg[lOptIndex]) {
          case 'h':
            mHelp = true;
            break;
          default:
            mFailureMessage =
                std::format("\"-{}\" is not a valid option", lArg[lOptIndex]);
            goto loop_end;
          }
        }
      } else {
        // Positional arguments are test paths
        mPaths.emplace_back(std::move(lArg));
      }
    }
  // Goto might not be the greatest idea but if we find a problem then we
  // want to end parsing with the first error message.
  loop_end:
  }

  // Print help message to the ostream with provided program name and optionally
  // exit with code 1.
  void printHelp(std::ostream &aStream) const {
    aStream << "TKoz SRTest -- Statically registered test library" << std::endl;
    aStream << std::format("Usage: {} [-h] [paths...]", mProgramName)
            << std::endl;
    aStream << "Test paths are in the form: path/to/dir/sourceFile:testName"
            << std::endl;
    aStream << "(start from repository root, do not include .cpp)" << std::endl;
    aStream << std::endl;
    aStream << "Options:" << std::endl;
    aStream << "  -h/--help Print help message and exit" << std::endl;
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
auto testsToRun(std::vector<std::string> const &aPaths)
    -> std::vector<tkoz::srtest::TestCaseInfo *> {
  auto const &sRegistry = tkoz::srtest::TestRegistry::instance();
  std::vector<tkoz::srtest::TestCaseInfo *> lResult;
  for (std::string const &lPath : aPaths) {
    std::size_t lSepPos = lPath.find(':');
    if (lSepPos == std::string::npos) {
      // Add all tests in files which begin with the specified path
      for (auto const &lTest : sRegistry) {
        if (!lTest->mFile.starts_with(lPath)) {
          continue;
        }
        auto const lIter =
            lTest->mFile.begin() +
            static_cast<std::string::difference_type>(lPath.size());
        if (lIter == lTest->mFile.end() || *lIter == '/') {
          lResult.push_back(lTest.get());
        }
      }
    } else {
      // Path and test name must match
      std::string lTestFile = lPath.substr(0, lSepPos);
      std::string lTestName = lPath.substr(lSepPos + 1);
      for (auto const &lTest : sRegistry)
        if (lTest->mFile == lTestFile && lTest->mName == lTestName) {
          lResult.push_back(lTest.get());
        }
    }
  }
  return lResult;
}

/// Convert a test category to a string
/// \param cat A test category
/// \return A string representation of the category
[[nodiscard]] static inline auto testTagsString(TestTags aTags) noexcept
    -> std::string {
  auto const lAllTags = aTags.allTags();
  if (lAllTags.empty()) {
    return "(no tags)";
  }
  std::string lResult = TestTags::tagString(lAllTags.front());
  for (std::size_t i = 1; i < lAllTags.size(); ++i) {
    lResult.push_back(',');
    lResult += TestTags::tagString(lAllTags[i]);
  }
  return lResult;
}

/// Test runner output. Write a sequence of values.
/// No separators are included. No new line is included.
template <typename... Ts>
  requires(sizeof...(Ts) > 0)
void infoWrite(Ts &&...aValues) {
  ((gInfoStream << aValues), ...);
}

/// Test runner output. Write a sequence of values in color.
/// TODO only write color if allowed by \c gCmdArgs .
template <typename... Ts>
  requires(sizeof...(Ts) > 0)
void infoWriteColored(const char *aColor, Ts &&...aValues) {
  gInfoStream << aColor;
  infoWrite(aValues...);
  gInfoStream << tkoz::srtest::cFmtReset;
}

/// Test runner output. Write a sequence of values and a new line.
/// No separators are included.
template <typename... Ts> void infoWriteLine(Ts &&...aValues) {
  if constexpr (sizeof...(Ts) > 0) {
    infoWrite(aValues...);
  }
  gInfoStream << std::endl;
}

/// Test runner output. Write a sequence of values in color with a new line.
/// TODO only write color if allowed by \c gCmdArgs .
template <typename... Ts>
void infoWriteColoredLine(const char *aColor, Ts &&...aValues) {
  if constexpr (sizeof...(Ts) > 0) {
    infoWriteColored(aColor, aValues...);
  }
  gInfoStream << std::endl;
}

} // namespace tkoz::srtest
// End of definitions for things the main runner uses

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

  auto const &sRegistry = tkoz::srtest::TestRegistry::instance();
  auto const &sAllTests = sRegistry.allTests();

  infoWriteColoredLine(tkoz::srtest::cFgBBlue,
                       "TKoz SRTest -- Statically Registered Test Library");
  infoWriteLine(std::format("Found {} registered tests", sAllTests.size()));

  auto lSelectedTests = testsToRun(gCmdArgs.paths());
  infoWriteLine(std::format("Selected {} tests to run", lSelectedTests.size()));

  std::size_t lNumRun = 0;
  std::size_t lNumSuccess = 0;
  std::size_t lNumFailed = 0;

  // We should have a monotonicity guarantee for timing performance so it is
  // better to use steady_clock instead of high_resolution_clock.
  using Clock = std::chrono::steady_clock;
  static_assert(Clock::is_steady);
  using TimePoint = decltype(Clock::now());
  using TimeDelta =
      decltype(std::declval<TimePoint>() - std::declval<TimePoint>());
  auto const fTimingsString = [](TimeDelta const &t) -> std::string {
    using namespace std::chrono;
    return std::format("{} / {} / {}", duration_cast<milliseconds>(t),
                       duration_cast<microseconds>(t),
                       duration_cast<nanoseconds>(t));
  };

  for (auto const &lTest : lSelectedTests) {
    infoWriteColored(tkoz::srtest::cFgBBlue, "Running");
    infoWriteLine(
        std::format(" {}:{} ({}, line {})", lTest->mFile, lTest->mName,
                    tkoz::srtest::testTagsString(lTest->mTags), lTest->mLine));
    tkoz::srtest::clearMessages();

    bool lSuccess = false;
    TimePoint lTimeStart;
    TimePoint lTimeFinish;
    std::optional<std::string> lFailureMessage;
    try {
      lTimeStart = Clock::now();
      lTest->run();
      lTimeFinish = Clock::now();
      lSuccess = true;
    } catch (tkoz::srtest::TestFailure const &exc) {
      lFailureMessage =
          std::format("{}Test failure{}: {}", tkoz::srtest::cFgBRed,
                      tkoz::srtest::cFmtReset, exc.message());
    } catch (std::exception const &exc) {
      lFailureMessage =
          std::format("{}Test failure{} ({}): {}", tkoz::srtest::cFgBRed,
                      tkoz::srtest::cFmtReset,
                      tkoz::srtest::typeName(&typeid(exc)), exc.what());
    } catch (...) {
#if defined(__GNUG__) || defined(__clang__)
      std::type_info const *const lType = abi::__cxa_current_exception_type();
#else
      std::type_info const *const lType = nullptr;
#endif
      lFailureMessage =
          std::format("{}Test failure{} ({})", tkoz::srtest::cFgBRed,
                      tkoz::srtest::cFmtReset, tkoz::srtest::typeName(lType));
    }
    ++lNumRun;
    if (lSuccess) {
      ++lNumSuccess;
      for (auto const &[lFailureOnly, lMessage] : tkoz::srtest::gTestMessages) {
        if (!lFailureOnly) {
          infoWriteLine(lMessage);
        }
      }
      infoWriteColored(tkoz::srtest::cFgBGreen, "Success");
      infoWriteLine(" (", fTimingsString(lTimeFinish - lTimeStart), ")");
    } else {
      lTimeFinish = Clock::now();
      ++lNumFailed;
      for (auto const &[lFailureOnly, lMessage] : tkoz::srtest::gTestMessages) {
        std::ignore = lFailureOnly;
        infoWriteLine(lMessage);
      }
      if (lFailureMessage.has_value()) {
        infoWriteLine(*lFailureMessage);
      }
      infoWriteColored(tkoz::srtest::cFgBRed, "Failure");
      infoWriteLine(" (", fTimingsString(lTimeFinish - lTimeStart), ")");
      // TODO determine if we should terminate after first failure with cmd args
      return 1;
    }
  }

  // Results
  infoWriteLine(std::format("Completed running {} tests", lNumRun));
  infoWriteColored(tkoz::srtest::cFgBGreen, "Successes");
  infoWriteLine(": ", lNumSuccess);
  if (lNumFailed > 0) {
    infoWriteColored(tkoz::srtest::cFgBRed, "Failures");
  } else {
    infoWrite("Failures");
  }
  infoWriteLine(": ", lNumFailed);

  // Give a nonzero exit code if any tests failed.
  return lNumFailed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
