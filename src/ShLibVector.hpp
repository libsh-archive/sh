// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
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
#ifndef SHLIBVECTOR_HPP
#define SHLIBVECTOR_HPP

#include "ShVector.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_USUAL_OPERATIONS(ShVector);
SH_SHLIB_USUAL_SUBTRACT(ShVector);

SH_SHLIB_LEFT_MATRIX_OPERATION(ShVector, operator|, M);

template<int N, int K1, int K2, typename T, bool S1, bool S2>
ShVariableN<1, T> operator|(const ShVector<N, K1, T, S1>& a,
                            const ShVector<N, K2, T, S2>& b)
{
  return dot(a, b);
}

template<int K1, int K2, typename T, bool S1>
ShVector<3, SH_VAR_TEMP, T, false> operator|(const ShMatrix<4, 4, K1, T>& m,
                                             const ShVector<3, K2, T, S1>& v)
{
  ShVector<3, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 3; i++) {
    t(i) = dot(m[i](0,1,2), v);
  }
  return t;
}

template<int K1, int K2, typename T, bool S1>
ShVector<2, SH_VAR_TEMP, T, false> operator|(const ShMatrix<3, 3, K1, T>& m,
                                             const ShVector<2, K2, T, S1>& v)
{
  ShVector<2, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 2; i++) {
    t(i) = dot(m[i](0,1), v);
  }
  return t;
}

template<int K1, int K2, typename T, bool S1>
ShVector<1, SH_VAR_TEMP, T, false> operator|(const ShMatrix<2, 2, K1, T>& m,
                                             const ShVector<1, K2, T, S1>& v)
{
  ShVector<1, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 1; i++) {
    t(i) = dot(m[i](0), v);
  }
  return t;
}

}

#endif
