// Copyright (C) 2021-2022 Petter Nilsson. MIT License.

#pragma once

/**
 * @file
 * @brief Fit Spline and Bspline from data.
 */

#include <ranges>

#include "bspline.hpp"
#include "spline.hpp"

SMOOTH_BEGIN_NAMESPACE

/**
 * @brief Spline specification.
 */
template<typename T>
concept SplineSpec = requires(T t) {
  // clang-format off
  { T::Degree } -> std::convertible_to<int>;
  { T::OptDeg } -> std::convertible_to<int>;
  { T::InnCnt } -> std::convertible_to<int>;
  { t.LeftDeg };
  { t.RghtDeg };
  // clang-format on
};

namespace spline_specs {

/**
 * @brief SplineSpec without boundary constraints
 *
 * @tparam K spline degree (must be 0 or 1)
 */
template<LieGroup G, int K>
struct NoConstraints
{
  /// @brief Polynomial degree
  static constexpr int Degree = K;
  /// @brief Optimization degree (absolute integral of derivative OptDeg is minimized)
  static constexpr int OptDeg = -1;
  /// @brief Number of derivatives to enforce continuity for
  static constexpr int InnCnt = int(K) - 1;

  /// @brief Degrees of left-side boundary constraints (no constraints)
  static constexpr std::array<int, 0> LeftDeg{};
  /// @brief Values of left-side boundary constraints
  std::array<Tangent<G>, 0> left_values{};

  /// @brief Degrees of right-side boundary constraints (no constraints)
  static constexpr std::array<int, 0> RghtDeg{};
  /// @brief Values of right-side boundary constraints
  std::array<Tangent<G>, 0> rght_values{};
};

/// @brief SplineSpec for a piecewise constant function
template<LieGroup G>
using PiecewiseConstant = NoConstraints<G, 0>;

/// @brief SplineSpec for a piecewise linear function
template<LieGroup G>
using PiecewiseLinear = NoConstraints<G, 1>;

/**
 * @brief SplineSpec for a cubic spline with two boundary conditions.
 *
 * @tparam P1 order of left boundary contraint (must be 1 or 2).
 * @tparam P2 order of right boundary contraint (must be 1 or 2).
 */
template<LieGroup G, int P1 = 2, int P2 = P1>
struct FixedDerCubic
{
  /// @brief Polynomial degree
  static constexpr int Degree = 3;
  /// @brief Optimization degree (absolute integral of derivative OptDeg is minimized)
  static constexpr int OptDeg = -1;
  /// @brief Number of derivatives to enforce continuity for
  static constexpr int InnCnt = 2;

  /// @brief Degrees of left-side boundary constraints: P1
  static constexpr std::array<int, 1> LeftDeg{P1};
  /// @brief Values of left-side boundary constraints
  std::array<Tangent<G>, 1> left_values{Tangent<G>::Zero()};

  /// @brief Degrees of right-side boundary constraints: P2
  static constexpr std::array<int, 1> RghtDeg{P2};
  /// @brief Values of right-side boundary constraints
  std::array<Tangent<G>, 1> rght_values{Tangent<G>::Zero()};
};

/**
 * @brief SplineSpec for optimized spline.
 *
 * @tparam K spline degree
 * @tparam O order to optimize
 * @tparam P continuity order
 */
template<LieGroup G, int K = 6, int O = 3, int P = 3>
struct MinDerivative
{
  /// @brief Polynomial degree
  static constexpr int Degree = K;
  /// @brief Optimization degree (absolute integral of derivative OptDeg is minimized)
  static constexpr int OptDeg = O;
  /// @brief Number of derivatives to enforce continuity for
  static constexpr int InnCnt = P;

  /// @brief Degrees of left-side boundary constraints: 1, 2, ..., P-1
  static constexpr std::array<int, std::size_t(P - 1)> LeftDeg = []() {
    std::array<int, std::size_t(P - 1)> ret;
    for (auto i = 0; i + 1 < P; ++i) { ret[static_cast<std::size_t>(i)] = i + 1; }
    return ret;
  }();

  /// @brief Values of left-side boundary constraints
  std::array<Tangent<G>, std::size_t(P - 1)> left_values = []() {
    std::array<Tangent<G>, std::size_t(P - 1)> ret;
    ret.fill(Tangent<G>::Zero());
    return ret;
  }();

  /// @brief Degrees of left-side boundary constraints: 1, 2, ..., P-1
  static constexpr std::array<int, std::size_t(P - 1)> RghtDeg = LeftDeg;
  /// @brief Values of right-side boundary constraints
  std::array<Tangent<G>, std::size_t(P - 1)> rght_values = left_values;
};

}  // namespace spline_specs

/**
 * @brief Find N degree K Bernstein polynomials p_i(t) for i = 0, ..., N s.t that satisfies
 * constraints and s.t.
 * \f[
 *   p_i(0) = 0 \\
 *   p_i(\delta t) = \delta x
 * \f]
 *
 * @param dt_r range of parameter differences \f$ \delta_t \f$
 * @param dx_r range of value differences \f$ \delta_x \f$
 * @param ss spline specification
 * @return vector \f$ \alpha \f$ of size (K + 1) * N s.t. \f$ \beta = \alpha_{i (K + 1): (i + 1) (K
 * + 1) } \f$ defines polynomial \f$ p_i \f$ as \f[ p_i(t) = \sum_{\nu = 0}^K \beta_\nu b_{\nu, k}
 * \left( \frac{t}{\delta t} \right), \f] where \f$ \delta t \f$ is the i:th member of \p dt_r.
 *
 * @note Allocates heap memory.
 */
Eigen::VectorXd
fit_spline_1d(std::ranges::sized_range auto && dt_r, std::ranges::sized_range auto && dx_r, const SplineSpec auto & ss);

/**
 * @brief Fit a Spline to given points.
 *
 * @tparam G LieGroup
 * @tparam K Spline degree
 *
 * @param ts range of times
 * @param gs range of values
 * @param ss spline specification
 *
 * @return Spline c s.t. \f$ c(t_i) = g_i \f$ for \f$(t_i, g_i) \in zip(ts, gs) \f$
 *
 * @note Allocates heap memory.
 */
auto fit_spline(
  std::ranges::random_access_range auto && ts, std::ranges::random_access_range auto && gs, const SplineSpec auto & ss);

/**
 * @brief Fit a cubic Spline with natural boundary conditions
 *
 * @param ts range of times
 * @param gs range of values
 * @return Spline c s.t. \f$ c(t_i) = g_i \f$ for \f$(t_i, g_i) \in zip(ts, gs) \f$
 *
 * @return Cubic spline that approximates data
 *
 * @note Allocates heap memory.
 */
auto fit_spline_cubic(std::ranges::range auto && ts, std::ranges::range auto && gs);

/**
 * @brief Fit a bpsline to data points \f$(t_i, g_i)\f$
 *        by solving the optimization problem
 *
 * \f[
 *   \min_{p}  \left\| p(t_i) - g_i \right\|^2
 * \f]
 *
 * @tparam K bspline degree
 * @param ts time values t_i (doubles, strictly increasing)
 * @param gs data values t_i
 * @param dt distance between spline control points
 *
 * @return BSpline of order K that approximates data
 *
 * @note Allocates heap memory.
 */
template<int K>
auto fit_bspline(std::ranges::range auto && ts, std::ranges::range auto && gs, const double dt);

SMOOTH_END_NAMESPACE
#include "detail/fit_impl.hpp"
