// Copyright (C) 2021-2022 Petter Nilsson. MIT License.

#pragma once

/**
 * @file
 * @brief Taylor tails of sin and cos of varying degrees.
 *
 * The functions take in a squared argument to play nicer with autodiff.
 */

#include <cmath>

#include "common.hpp"

SMOOTH_BEGIN_NAMESPACE

namespace detail {
template<typename S>
S cos_2(const S & x2)
{
  using std::cos, std::sqrt;

  if (x2 > S(eps2)) {
    const S x = sqrt(x2);
    return (cos(x) - S(1)) / x2;
  } else {
    return -S(1) / S(2) + x2 / S(24) - x2 * x2 / S(720);
  }
}

template<typename S>
S sin_3(const S & x2)
{
  using std::sin, std::sqrt;

  if (x2 > S(eps2)) {
    const S x = sqrt(x2);
    return (sin(x) - x) / (x2 * x);
  } else {
    return -S(1) / S(6) + x2 / S(120) - x2 * x2 / S(5040);
  }
}

template<typename S>
S cos_4(const S & x2)
{
  using std::cos, std::sqrt;

  if (x2 > S(eps2)) {
    const S x = sqrt(x2);
    return (cos(x) - S(1) + x2 / S(2)) / (x2 * x2);
  } else {
    return S(1) / S(24) - x2 / S(720) + (x2 * x2) / S(40320);
  }
}

template<typename S>
S sin_5(const S & x2)
{
  using std::sin, std::sqrt;

  if (x2 > S(eps2)) {
    const S x = sqrt(x2);
    return (sin(x) - x + x2 * x / 6) / (x2 * x2 * x);
  } else {
    return S(1) / S(120) - x2 / S(5040) + x2 * x2 / S(362880);
  }
}

template<typename S>
S cos_6(const S & x2)
{
  using std::cos, std::sqrt;

  const S x4 = x2 * x2;
  if (x2 > S(eps2)) {
    const S x = sqrt(x2);
    return (cos(x) - S(1) + x2 / S(2) - x4 / S(24)) / (x4 * x2);
  } else {
    return -S(1) / S(720) + x2 / S(40320) - x4 / S(3628800);
  }
}

}  // namespace detail

SMOOTH_END_NAMESPACE
