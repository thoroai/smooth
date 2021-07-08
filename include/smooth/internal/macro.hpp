// smooth: Lie Theory for Robotics
// https://github.com/pettni/smooth
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
//
// Copyright (c) 2021 Petter Nilsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SMOOTH__MACRO_HPP_
#define SMOOTH__MACRO_HPP_

namespace smooth {

#define SMOOTH_INHERIT_TYPEDEFS                            \
  using Base::is_mutable;                                  \
  using Base::Dof;                                         \
  using Base::RepSize;                                     \
  /*! Scalar type. */                                      \
  using Scalar = typename Base::Scalar;                    \
  /*! Tangent type (Eigen column vector of length Dof). */ \
  using Tangent       = typename Base::Tangent;            \
  using Base::operator=;                                   \
  using Base::operator*;                                   \
                                                           \
  static_assert(true, "")

#define SMOOTH_GROUP_API(X)                                   \
public:                                                       \
  SMOOTH_INHERIT_TYPEDEFS;                                    \
  /**< Construct uninitialized object */                      \
  X() = default;                                              \
  /*! Copy constructor */                                     \
  X(const X &) = default;                                     \
  /*! Move contructor */                                      \
  X(X &&) = default;                                          \
  /*! Copy assignment */                                      \
  X & operator=(const X &) = default;                         \
  /*! Move assignment */                                      \
  X & operator=(X &&) = default;                              \
  ~X()                = default;                              \
  /*! Copy assignment from other storage type */              \
  template<typename OtherDerived>                             \
  X(const X##Base<OtherDerived> & o)                          \
  {                                                           \
    coeffs() = static_cast<const OtherDerived &>(o).coeffs(); \
  }                                                           \
                                                              \
  /*! Underlying storage is an Eigen matrix */                \
  using Storage = Eigen::Matrix<Scalar, RepSize, 1>;          \
                                                              \
  /*! Access underlying Eigen::Matrix */                      \
  Storage & coeffs() { return coeffs_; }                      \
  /*! Const access underlying Eigen::Matrix */                \
  const Storage & coeffs() const { return coeffs_; }          \
                                                              \
  /*! Access raw pointer */                                   \
  Scalar * data() { return coeffs_.data(); }                  \
  /*! Const access raw pointer */                             \
  const Scalar * data() const { return coeffs_.data(); }      \
                                                              \
private:                                                      \
  Storage coeffs_;                                            \
                                                              \
  static_assert(true, "")

#define SMOOTH_MAP_API(X)                                        \
public:                                                          \
  SMOOTH_INHERIT_TYPEDEFS;                                       \
  /**                                                            \
   * @brief Map memory as Lie type                               \
   *                                                             \
   * @param p pointer to memory                                  \
   **/                                                           \
  X(Scalar * p) : coeffs_(p) {}                                  \
  /*! Copy constructor */                                        \
  X(const X &) = default;                                        \
  /*! Move constructor */                                        \
  X(X &&) = default;                                             \
  /*! Copy assignment */                                         \
  X & operator=(const X &) = default;                            \
  /*! Move assignment */                                         \
  X & operator=(X &&) = default;                                 \
  ~X()                = default;                                 \
                                                                 \
  /*! Underlying storage is Eigen map */                         \
  using Storage = Eigen::Map<Eigen::Matrix<Scalar, RepSize, 1>>; \
                                                                 \
  /*! Access underlying Eigen::Map */                            \
  Storage & coeffs() { return coeffs_; }                         \
  /*! Const access underlying Eigen::Map */                      \
  const Storage & coeffs() const { return coeffs_; }             \
                                                                 \
  /*! Access raw pointer */                                      \
  Scalar * data() { return coeffs_.data(); }                     \
  /*! Const access raw pointer */                                \
  const Scalar * data() const { return coeffs_.data(); }         \
                                                                 \
private:                                                         \
  Storage coeffs_;                                               \
                                                                 \
  static_assert(true, "")

#define SMOOTH_CONST_MAP_API(X)                                        \
public:                                                                \
  SMOOTH_INHERIT_TYPEDEFS;                                             \
  /**                                                                  \
   * @brief Const map memory as Lie type                               \
   *                                                                   \
   * @param p pointer to memory                                        \
   **/                                                                 \
  X(const Scalar * p) : coeffs_(p) {}                                  \
  /*! Copy constructor */                                              \
  X(const X &) = default;                                              \
  /*! Move constructor */                                              \
  X(X &&) = default;                                                   \
  /*! Copy assignment */                                               \
  X & operator=(const X &) = default;                                  \
  /*! Move assignment */                                               \
  X & operator=(X &&) = default;                                       \
  ~X()                = default;                                       \
                                                                       \
  /*! Underlying storage is Eigen const map */                         \
  using Storage = Eigen::Map<const Eigen::Matrix<Scalar, RepSize, 1>>; \
                                                                       \
  /*! Const access underlying Eigen::Map */                            \
  const Storage & coeffs() const { return coeffs_; }                   \
                                                                       \
  /*! Const access raw pointer */                                      \
  const Scalar * data() const { return coeffs_.data(); }               \
                                                                       \
private:                                                               \
  Storage coeffs_;                                                     \
                                                                       \
  static_assert(true, "")

}  // namespace smooth

#endif  // SMOOTH__MACRO_HPP_
