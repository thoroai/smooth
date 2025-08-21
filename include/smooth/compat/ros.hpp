// Copyright (C) 2022 Petter Nilsson. MIT License.

#pragma once

/**
 * @file
 * @brief ROS message compatablity header.
 */

#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/transform.hpp>

#include "smooth/se3.hpp"

using namespace geometry_msgs::msg;

// Statically check that messages are laid out as expected in memory

// Vector3
static_assert(std::is_same_v<decltype(Vector3::x), double>);
static_assert(std::is_same_v<decltype(Vector3::y), double>);
static_assert(std::is_same_v<decltype(Vector3::z), double>);

static_assert(offsetof(Vector3, x) == 0);
static_assert(offsetof(Vector3, y) == sizeof(double));
static_assert(offsetof(Vector3, z) == 2 * sizeof(double));

// Point
static_assert(std::is_same_v<decltype(Point::x), double>);
static_assert(std::is_same_v<decltype(Point::y), double>);
static_assert(std::is_same_v<decltype(Point::z), double>);

static_assert(offsetof(Point, x) == 0);
static_assert(offsetof(Point, y) == sizeof(double));
static_assert(offsetof(Point, z) == 2 * sizeof(double));

// Quaternion
static_assert(std::is_same_v<decltype(Quaternion::x), double>);
static_assert(std::is_same_v<decltype(Quaternion::y), double>);
static_assert(std::is_same_v<decltype(Quaternion::z), double>);
static_assert(std::is_same_v<decltype(Quaternion::w), double>);

static_assert(offsetof(Quaternion, x) == 0);
static_assert(offsetof(Quaternion, y) == sizeof(double));
static_assert(offsetof(Quaternion, z) == 2 * sizeof(double));
static_assert(offsetof(Quaternion, w) == 3 * sizeof(double));

// Pose
static_assert(std::is_same_v<decltype(Pose::position), Point>);
static_assert(std::is_same_v<decltype(Pose::orientation), Quaternion>);
static_assert(offsetof(Pose, position) == 0);
static_assert(offsetof(Pose, orientation) == sizeof(Point));

// Tranform
static_assert(std::is_same_v<decltype(Transform::translation), Vector3>);
static_assert(std::is_same_v<decltype(Transform::rotation), Quaternion>);
static_assert(offsetof(Transform, translation) == 0);
static_assert(offsetof(Transform, rotation) == sizeof(Vector3));

// generic

//! Map message DATATYPE as implementation LIETYPE with CRTP base BASETYPE
#define CREATE_MAPS(DATATYPE, LIETYPE, BASETYPE)                                             \
  /*! @brief Specialize liebase_info       . */                                              \
  template<>                                                                                 \
  struct smooth::liebase_info<smooth::Map<DATATYPE>> : public liebase_info<LIETYPE>          \
  {};                                                                                        \
                                                                                             \
  /*! @brief Memory mapping of ROS message as Lie group type. */                             \
  template<>                                                                                 \
  class smooth::Map<DATATYPE> : public BASETYPE<smooth::Map<DATATYPE>>                       \
  {                                                                                          \
    using Base = BASETYPE<smooth::Map<DATATYPE>>;                                            \
                                                                                             \
  public:                                                                                    \
    /*! @brief Define types. */                                                              \
    SMOOTH_INHERIT_TYPEDEFS;                                                                 \
                                                                                             \
    /*! Map message as Lie group type. */                                                    \
    explicit Map(DATATYPE & msg) : m_coeffs(reinterpret_cast<double *>(&msg)) {}             \
    /*! Underlying storage is Eigen::Map */                                                  \
    using Storage = Eigen::Map<Eigen::Matrix<double, RepSize, 1>>;                           \
    /*! Access underlying Eigen::Map */                                                      \
    Storage & coeffs() { return m_coeffs; }                                                  \
    /*! Const access underlying Eigen::Map */                                                \
    const Storage & coeffs() const { return m_coeffs; }                                      \
    /*! Access raw pointer */                                                                \
    Scalar * data() { return m_coeffs.data(); }                                              \
    /*! Const access raw pointer */                                                          \
    const Scalar * data() const { return m_coeffs.data(); }                                  \
                                                                                             \
  private:                                                                                   \
    Storage m_coeffs;                                                                        \
  };                                                                                         \
                                                                                             \
  /*! @brief Specialize liebase_info. */                                                     \
  template<>                                                                                 \
  struct smooth::liebase_info<smooth::Map<const DATATYPE>> : public liebase_info<LIETYPE>    \
  {                                                                                          \
    /*! @brief Const mapping is not mutable. */                                              \
    static constexpr bool is_mutable = false;                                                \
  };                                                                                         \
                                                                                             \
  /*! @brief Const memory mapping of ROS message as Lie group type. */                       \
  template<>                                                                                 \
  class smooth::Map<const DATATYPE> : public BASETYPE<smooth::Map<const DATATYPE>>           \
  {                                                                                          \
    using Base = BASETYPE<smooth::Map<const DATATYPE>>;                                      \
                                                                                             \
  public:                                                                                    \
    /*! @brief Define types. */                                                              \
    SMOOTH_INHERIT_TYPEDEFS;                                                                 \
                                                                                             \
    /*! Const map message as Lie group type. */                                              \
    explicit Map(const DATATYPE & msg) : m_coeffs(reinterpret_cast<const double *>(&msg)) {} \
    /*! Underlying storage is Eigen const Map */                                             \
    using Storage = Eigen::Map<const Eigen::Matrix<double, RepSize, 1>>;                     \
    /*! Access underlying Eigen::Map */                                                      \
    const Storage & coeffs() const { return m_coeffs; }                                      \
    /*! Access raw pointer */                                                                \
    const Scalar * data() const { return m_coeffs.data(); }                                  \
                                                                                             \
  private:                                                                                   \
    Storage m_coeffs;                                                                        \
  };                                                                                         \
                                                                                             \
  static_assert(true, "")

CREATE_MAPS(geometry_msgs::msg::Quaternion, smooth::SO3d, smooth::SO3Base);
CREATE_MAPS(geometry_msgs::msg::Pose, smooth::SE3d, smooth::SE3Base);
CREATE_MAPS(geometry_msgs::msg::Transform, smooth::SE3d, smooth::SE3Base);
