// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHMATH_HPP
#define SHMATH_HPP

/** @file ShMath.hpp
 * Some C99 math functions that windows/apple don't have
 */

#include <cmath>

namespace {
#ifdef _WIN32
  inline float log2f(float a) { return logf(a) / logf(2.0f); }
  inline float exp2f(float a) { return powf(2.0f, a); }
  inline float exp10f(float a) { return powf(10.0f, a); }
#endif

#ifdef __CYGWIN__
# undef log2
#endif

#ifdef __APPLE__
  inline float exp10f(float a) { return powf(10.0f, a); }
  inline double exp10(double a) { return pow(10.0, a); }
#endif
}

#endif
