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
#ifndef SHLIBTRIGIMPL_HPP
#define SHLIBTRIGIMPL_HPP

#include "LibTrig.hpp"
#include "Attrib.hpp"
#include "Instructions.hpp"

namespace SH {

template<int N, typename T>
Generic<N, T> acos(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shACOS(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> asin(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shASIN(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> atan(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shATAN(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> atan2(const Generic<N, T>& y, const Generic<N, T>& x)
{
  Attrib<N, SH_TEMP, T> t;
  shATAN2(t, y, x);
  return t;
}

template<int N, typename T>
Generic<N, T> cos(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shCOS(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> sin(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shSIN(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> tan(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shTAN(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> cosh(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shCOSH(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> sinh(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shSINH(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> tanh(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shTANH(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> acosh(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shACOSH(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> asinh(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shASINH(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> atanh(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shATANH(t, var);
  return t;
}

}

#endif
