#pragma once

#include <tkoz/ff/Types.hpp>
#include <tkoz/ff/fpMath/CMath.hpp>

#include <utility>

namespace tkoz::ff::fpMath {

/// \brief Simple class for fixed rotations in the 2D plane.
template <cFpType T> class Fixed2dRotation {
private:
  // These would have been const but they must be initialized after computing
  // simulatneous sincos so the initializer list does not work.
  T mSinTheta;
  T mCosTheta;

public:
  /// Create an object to handle rotations for an angle \a theta.
  /// The rotation is counterclockwise in the standard coordinate plane.
  /// \param theta The rotation angle in radians.
  [[nodiscard]] inline explicit Fixed2dRotation(T theta) noexcept {
    const auto [s, c] = cmathSinCos(theta);
    mSinTheta = s;
    mCosTheta = c;
  }

  /// Access the computed sin(theta).
  /// \return The computed sin(theta).
  [[nodiscard]] inline T sinTheta() const noexcept { return mSinTheta; }

  /// Access the computed cos(theta).
  /// \return The computed cos(theta).
  [[nodiscard]] inline T cosTheta() const noexcept { return mCosTheta; }

  /// Apply the rotation in place
  /// \param x The x coordinate
  /// \param y The y coordinate
  inline void apply(T &x, T &y) const noexcept {
    const T xx = (x * mCosTheta) - (y * mSinTheta);
    const T yy = (x * mSinTheta) + (y * mCosTheta);
    x = xx;
    y = yy;
  }

  /// Rotate a point in the plane
  /// \param x The x coordinate
  /// \param y The y coordinate
  /// \return The new point as a pair \a [x,y]
  [[nodiscard]] inline auto rotate(T x, T y) const noexcept -> std::pair<T, T> {
    return std::make_pair((x * mCosTheta) - (y * mSinTheta),
                          (x * mSinTheta) + (y * mCosTheta));
  }
};

/// Deduction guide
template <typename T> Fixed2dRotation(T) -> Fixed2dRotation<T>;

} // namespace tkoz::ff::fpMath
