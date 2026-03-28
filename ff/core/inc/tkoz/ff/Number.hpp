#pragma once

#include <tkoz/ff/Types.hpp>

#include <type_traits>
#include <utility>

namespace tkoz::ff {

/// \brief Wrapper for float/double to ensure math policies are used properly
/// and operations do not mix different precision.
/// \tparam NumT Underlying number type (float or double).
///
/// We should be able to specialize this for half or double double later on.
/// - half precision probably will not give good results
/// - double double probably will just be slower and no better than double
/// - float may be enough and the right choice for performance
/// Currently we assume in some places that Number<...>::FpType is a primitive
/// float/double so some work would be required to support more options.
template <cPrimitiveFpType NumT> class Number {
private:
  NumT mValue;

public:
  /// The underlying floating point type.
  using FpType = NumT;

  /// Default constructor (uninitialized).
  [[nodiscard]] inline constexpr Number() noexcept = default;

  /// Create a \c Number from the underlying floating point type.
  /// \param value Number value
  /// \note Templated for type safety to avoid mixing floating point types.
  template <cPrimitiveFpType ArgNumT>
    requires std::is_same_v<NumT, ArgNumT>
  [[nodiscard]] inline constexpr /*implicit*/ Number(ArgNumT value) noexcept
      : mValue(value) {}

  /// Assign a value from the underlying floating point type.
  /// \param value Number value
  /// \note Templated for type safety to avoid mixing floating point types.
  template <cPrimitiveFpType ArgNumT>
    requires std::is_same_v<NumT, ArgNumT>
  inline constexpr auto operator=(ArgNumT value) noexcept -> Number & {
    mValue = value;
    return *this;
  }

  /// Convert to the underlying number type.
  /// \return The underlying floating point value of this \c Number
  /// \note Templated for type safety to avoid mixing floating point types.
  template <cPrimitiveFpType ToNumT>
    requires std::is_same_v<NumT, ToNumT>
  [[nodiscard]] inline constexpr explicit operator ToNumT() const noexcept {
    return mValue;
  }

  /// Access the wrapped primitive value.
  /// \return The underlying floating point value of this \c Number
  [[nodiscard]] inline constexpr auto value() const noexcept -> NumT {
    return mValue;
  }

  /// Replace the wrapped primitive value.
  /// \param value The new value.
  /// \note Templated for type safety to avoid mixing floating point types.
  template <cPrimitiveFpType ArgNumT>
    requires std::is_same_v<NumT, ArgNumT>
  inline constexpr void setValue(ArgNumT value) noexcept {
    mValue = value;
  }

  /// Rule of 5 things ///

  [[nodiscard]] inline constexpr Number(Number const &other) noexcept = default;
  [[nodiscard]] inline constexpr Number(Number &&other) noexcept = default;
  inline constexpr auto operator=(Number const &other) noexcept
      -> Number & = default;
  inline constexpr auto operator=(Number &&other) noexcept
      -> Number & = default;
  ~Number() noexcept = default;

  /// In place arithmetic ///

  inline constexpr auto operator+=(Number other) noexcept -> Number & {
    mValue += other.mValue;
    return *this;
  }

  inline constexpr auto operator-=(Number other) noexcept -> Number & {
    mValue -= other.mValue;
    return *this;
  }

  inline constexpr auto operator*=(Number other) noexcept -> Number & {
    mValue *= other.mValue;
    return *this;
  }

  inline constexpr auto operator/=(Number other) noexcept -> Number & {
    mValue /= other.mValue;
    return *this;
  }

  /// Unary operators ///

  [[nodiscard]] inline constexpr auto operator+() const noexcept -> Number {
    return Number(mValue);
  }

  [[nodiscard]] inline constexpr auto operator-() const noexcept -> Number {
    return Number(-mValue);
  }

  /// Binary arithmetic ///

  [[nodiscard]] friend inline constexpr auto operator+(Number left,
                                                       Number right) noexcept
      -> Number {
    return Number(left.mValue + right.mValue);
  }

  [[nodiscard]] friend inline constexpr auto operator-(Number left,
                                                       Number right) noexcept
      -> Number {
    return Number(left.mValue - right.mValue);
  }

  [[nodiscard]] friend inline constexpr auto operator*(Number left,
                                                       Number right) noexcept
      -> Number {
    return Number(left.mValue * right.mValue);
  }

  [[nodiscard]] friend inline constexpr auto operator/(Number left,
                                                       Number right) noexcept
      -> Number {
    return Number(left.mValue / right.mValue);
  }

  /// Compare operators ///

  [[nodiscard]] friend inline constexpr auto operator==(Number left,
                                                        Number right) -> bool {
    return left.mValue == right.mValue;
  }

  [[nodiscard]] friend inline constexpr auto operator!=(Number left,
                                                        Number right) -> bool {
    return left.mValue != right.mValue;
  }

  [[nodiscard]] friend inline constexpr auto operator<(Number left,
                                                       Number right) -> bool {
    return left.mValue < right.mValue;
  }

  [[nodiscard]] friend inline constexpr auto operator>(Number left,
                                                       Number right) -> bool {
    return left.mValue > right.mValue;
  }

  [[nodiscard]] friend inline constexpr auto operator<=(Number left,
                                                        Number right) -> bool {
    return left.mValue <= right.mValue;
  }

  [[nodiscard]] friend inline constexpr auto operator>=(Number left,
                                                        Number right) -> bool {
    return left.mValue >= right.mValue;
  }

  [[nodiscard]] friend inline constexpr auto operator<=>(Number left,
                                                         Number right)
      -> decltype(std::declval<NumT>() <=> std::declval<NumT>()) {
    return left.mValue <=> right.mValue;
  }
};

} // namespace tkoz::ff
