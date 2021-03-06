// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#ifndef SHLIBDERIVIMPL_HPP
#define SHLIBDERIVIMPL_HPP

#include "LibDeriv.hpp"

namespace SH {

template<int N, typename T>
Generic<N, T> dx(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shDX(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> dy(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shDY(t, var);
  return t;
}

template<int N, typename T>
inline
Generic<N, T> fwidth(const Generic<N, T>& var)
{
  return max(abs(dx(var)), abs(dy(var)));
}

template<typename T>
inline
Generic<2, T> gradient(const Generic<1, T>& var)
{
  return Attrib2f(dx(var), dy(var));
}

template<int N, typename T>
Matrix<2, N, SH_TEMP, T> jacobian(const Generic<N, T>& var)
{
  Matrix<2, N, SH_TEMP, T> ret;
  ret[0] = dx(var);
  ret[1] = dy(var);
  return ret;
}



}

#endif
