// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Viberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHLIBDERIVIMPL_HPP
#define SHLIBDERIVIMPL_HPP

#include "ShLibDeriv.hpp"

namespace SH {

template<int N, ShValueType V>
inline
ShGeneric<N, V> dx(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shDX(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> dy(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shDY(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> fwidth(const ShGeneric<N, V>& var)
{
  return max(abs(dx(var)), abs(dy(var)));
}

template<ShValueType V>
inline
ShGeneric<2, V> gradient(const ShGeneric<1, V>& var)
{
  return ShAttrib2f(dx(var), dy(var));
}

template<int N, ShValueType V>
inline
ShMatrix<2, N, SH_TEMP, V> jacobian(const ShGeneric<N, V>& var)
{
  ShMatrix<2, N, SH_TEMP, V> ret;
  ret[0] = dx(var);
  ret[1] = dy(var);
  return ret;
}



}

#endif
