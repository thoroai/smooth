// Copyright (C) 2021-2022 Petter Nilsson. MIT License.

#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "detail/macro.hpp"
#include "detail/so3.hpp"
#include "lie_group_base.hpp"

SMOOTH_BEGIN_NAMESPACE

// \cond
template<typename Scalar>
class SO2;
// \endcond

/**
 * @brief Base class for SO3 Lie group types
 *
 * Internally represented as a member of \f$ \mathbb{S}^3 \f$ (unit quaternions).
 *
 * Memory layout
 * -------------
 *
 * - Group:    \f$ \mathbf{x} = [q_x, q_y, q_z, q_w] \f$ (same as Eigen quaternion).
 * - Tangent:  \f$ \mathbf{a} = [\omega_x, \omega_y, \omega_z] \f$
 *
 * Constraints
 * -----------
 *
 * - Group:   \f$ q_x^2 + q_y^2 + q_z^2 + q_w^2 = 1, q_w >= 0 \f$
 * - Tangent: \f$ -\pi < \omega_x, \omega_y, \omega_z \leq \pi \f$
 *
 * Lie group matrix form
 * ---------------------
 *
 * 3x3 rotation matrix
 *
 * Lie algebra matrix form
 * -----------------------
 *
 * \f[
 * \mathbf{a}^\wedge =
 * \begin{bmatrix}
 *   0 & -\omega_z & \omega_y \\
 *  \omega_z &   0 & -\omega_x \\
 *  -\omega_y &   \omega_x & 0 \\
 * \end{bmatrix} \in \mathbb{R}^{3 \times 3}
 * \f]
 */
template<typename _Derived>
class SO3Base : public LieGroupBase<_Derived>
{
  using Base = LieGroupBase<_Derived>;

protected:
  SO3Base() = default;

public:
  SMOOTH_INHERIT_TYPEDEFS;

  /**
   * @brief Access quaterion.
   */
  Eigen::Map<Eigen::Quaternion<Scalar>> quat()
    requires is_mutable
  {
    return Eigen::Map<Eigen::Quaternion<Scalar>>(static_cast<_Derived &>(*this).data());
  }

  /**
   * @brief Access const quaterion.
   */
  Eigen::Map<const Eigen::Quaternion<Scalar>> quat() const
  {
    return Eigen::Map<const Eigen::Quaternion<Scalar>>(static_cast<const _Derived &>(*this).data());
  }

  /**
   * @brief Return euler angles.
   *
   * @param i1, i2, i3 euler angle axis convention (\f$0=x, 1=y, 2=z\f$).
   *        Default values correspond to ZYX rotation.
   *
   * Returned angles a1, a2, a3 are s.t. rotation is described by
   * \f$ Rot_{i_1}(a_1) * Rot_{i_2}(a_2) * Rot_{i_3}(a_3). \f$
   * where \f$ Rot_i(a) \f$ rotates an angle \f$a\f$ around the \f$i\f$:th axis.
   */
  Eigen::Vector3<Scalar> eulerAngles(Eigen::Index i1 = 2, Eigen::Index i2 = 1, Eigen::Index i3 = 0) const
  {
    return quat().toRotationMatrix().eulerAngles(i1, i2, i3);
  }

  /**
   * @brief Rotation action on 3D vector.
   */
  template<typename EigenDerived>
  Eigen::Vector3<Scalar> operator*(const Eigen::MatrixBase<EigenDerived> & v) const
  {
    return quat() * v;
  }

  /**
   * @brief Jacobian of rotation action w.r.t. group.
   *
   * \f[
   *   \mathrm{d}^r (X v)_X
   * \f]
   */
  template<typename EigenDerived>
  Eigen::Matrix3<Scalar> dr_action(const Eigen::MatrixBase<EigenDerived> & v) const
  {
    return -Base::matrix() * Base::hat(v);
  }

  /**
   * @brief Project to SO2.
   *
   * This keeps the yaw/z axis component of the rotation.
   *
   * @note SO2 header must be included.
   */
  SO2<Scalar> project_so2() const
  {
    using std::atan2;

    const auto q = quat();
    Scalar yaw =
      atan2(Scalar(2) * (q.w() * q.z() + q.x() * q.y()), Scalar(1) - Scalar(2) * (q.y() * q.y() + q.z() * q.z()));
    return SO2<Scalar>(yaw);
  }
};

// STORAGE TYPE TRAITS

// \cond
template<typename _Scalar>
class SO3;
// \endcond

// \cond
template<typename _Scalar>
struct liebase_info<SO3<_Scalar>>
{
  static constexpr bool is_mutable = true;

  using Impl   = SO3Impl<_Scalar>;
  using Scalar = _Scalar;

  template<typename NewScalar>
  using PlainObject = SO3<NewScalar>;
};
// \endcond

/**
 * @brief Storage implementation of SO3 Lie group.
 *
 * @see SO3Base for group API.
 */
template<typename _Scalar>
class SO3 : public SO3Base<SO3<_Scalar>>
{
  using Base = SO3Base<SO3<_Scalar>>;

  SMOOTH_GROUP_API(SO3);

public:
  /**
   * @brief Construct from quaternion.
   *
   * @param quat Eigen quaternion.
   *
   * @note Input is normalized inside constructor.
   */
  template<typename Derived>
  explicit SO3(const Eigen::QuaternionBase<Derived> & quat) : m_coeffs(quat.normalized().coeffs())
  {
    if (m_coeffs(3) < 0) { m_coeffs *= Scalar(-1); }
  }

  /**
   * @brief SO3 representing rotation around the x axis
   * @param angle angle of rotation [rad]
   */
  static SO3 rot_x(const Scalar & angle)
  {
    using std::cos, std::sin;

    SO3 ret;
    ret.coeffs() << sin(angle / 2), Scalar(0), Scalar(0), cos(angle / 2);
    if (ret.coeffs()(3) < 0) { ret.coeffs() *= Scalar(-1); }
    return ret;
  }

  /**
   * @brief SO3 representing rotation around the y axis
   * @param angle angle of rotation [rad]
   */
  static SO3 rot_y(const Scalar & angle)
  {
    using std::cos, std::sin;

    SO3 ret;
    ret.coeffs() << Scalar(0), sin(angle / 2), Scalar(0), cos(angle / 2);
    if (ret.coeffs()(3) < 0) { ret.coeffs() *= Scalar(-1); }
    return ret;
  }

  /**
   * @brief SO3 representing rotation around the z axis
   * @param angle angle of rotation [rad]
   */
  static SO3 rot_z(const Scalar & angle)
  {
    using std::cos, std::sin;

    SO3 ret;
    ret.coeffs() << Scalar(0), Scalar(0), sin(angle / 2), cos(angle / 2);
    if (ret.coeffs()(3) < 0) { ret.coeffs() *= Scalar(-1); }
    return ret;
  }
};

// \cond
template<typename _Scalar>
struct liebase_info<Map<SO3<_Scalar>>> : public liebase_info<SO3<_Scalar>>
{};
// \endcond

/**
 * @brief Memory mapping of SO3 Lie group.
 *
 * @see SO3Base for group API.
 */
template<typename _Scalar>
class Map<SO3<_Scalar>> : public SO3Base<Map<SO3<_Scalar>>>
{
  using Base = SO3Base<Map<SO3<_Scalar>>>;

  SMOOTH_MAP_API();
};

// \cond
template<typename _Scalar>
struct liebase_info<Map<const SO3<_Scalar>>> : public liebase_info<SO3<_Scalar>>
{
  static constexpr bool is_mutable = false;
};
// \endcond

/**
 * @brief Const memory mapping of SO3 Lie group.
 *
 * @see SO3Base for group API.
 */
template<typename _Scalar>
class Map<const SO3<_Scalar>> : public SO3Base<Map<const SO3<_Scalar>>>
{
  using Base = SO3Base<Map<const SO3<_Scalar>>>;

  SMOOTH_CONST_MAP_API();
};

using SO3f = SO3<float>;   ///< SO3 with float scalar representation
using SO3d = SO3<double>;  ///< SO3 with double scalar representation

SMOOTH_END_NAMESPACE

// Std format
#if __has_include(<format>)
#include <format>
#include <string>

template<class Scalar>
struct std::formatter<smooth::SO3<Scalar>>
{
  std::string m_format;

  constexpr auto parse(std::format_parse_context & ctx)
  {
    m_format = "{:";
    for (auto it = ctx.begin(); it != ctx.end(); ++it) {
      char c = *it;
      m_format += c;
      if (c == '}') return it;
    }
    return ctx.end();
  }

  auto format(const smooth::SO3<Scalar> & obj, std::format_context & ctx) const
  {
    const auto fmtSting = std::format("[{0}, {0}, {0}, {0}]", m_format);
    return std::vformat_to(
      ctx.out(), fmtSting, std::make_format_args(obj.quat().w(), obj.quat().x(), obj.quat().y(), obj.quat().z()));
  }
};

#endif
