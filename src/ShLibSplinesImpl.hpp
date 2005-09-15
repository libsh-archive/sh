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
#ifndef SHLIBSPLINESIMPL_HPP
#define SHLIBSPLINESIMPL_HPP

#include "ShLibSplines.hpp"
#include "ShDebug.hpp"

namespace SH {

template<int N, typename T>
ShGeneric<N, T> bernstein(const ShGeneric<1, T>& a)
{
  ShAttrib<N, SH_TEMP, T> result;
  if (4 == N) {
    ShAttrib<1, SH_TEMP, T> it = ShAttrib<1,SH_TEMP, T>(1.0f) - a;
    result(0) = it*it*it;
    result(1) = 3.0*it*it*a;
    result(2) = 3.0*it*a*a;
    result(3) = a*a*a;
  }
  else {
    // TODO: implement the real thing for the general case
    SH_DEBUG_WARN("bernstein is not fully implemented yet");
    for (int i=0; i < N; i++) {
      result[i] = a[0];
    }
  }
  return result;
}

template <int N, typename T>
ShGeneric<N, T> bezier(const ShGeneric<1, T>& t, const ShGeneric<N, T>& p)
{
  ShAttrib<N, SH_TEMP, T> B = bernstein<N>(t(0));
  ShAttrib<N, SH_TEMP, T> r;
  r(0) = B[0] * p[0];
  for (int i=1; i < N; i++) {
    r(i) = B[i] * p[i];
  }
  return r;
}

}

#endif
