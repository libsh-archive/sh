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
#ifndef SHLIBMISCIMPL_HPP
#define SHLIBMISCIMPL_HPP

#include "ShLibMisc.hpp"
#include "ShInstructions.hpp"
#include "ShProgram.hpp"

namespace SH {

template<int M, int N, ShValueType V> 
inline
ShGeneric<M, V> cast(const ShGeneric<N, V>& a)
{
  int copySize = std::min(M, N);
  ShAttrib<M, SH_TEMP, V> result;

  int* indices = new int[copySize];
  for(int i = 0; i < copySize; ++i) indices[i] = i;
  if(M < N) {
    result = a.template swiz<M>(indices);
  } else if( M > N ) {
    result.template swiz<N>(indices) = a;
  } else { // M == N
    shASN(result, a);
  }
  delete [] indices;
  return result;
}

template<int M> 
inline
ShGeneric<M, SH_DOUBLE> cast(double a)
{
  return cast<M>(ShAttrib<1, SH_CONST, SH_DOUBLE>(a));
}

template<int M, int N, ShValueType V> 
inline
ShGeneric<M, V> fillcast(const ShGeneric<N, V>& a)
{
  if( M <= N ) return cast<M>(a);
  int indices[M];
  for(int i = 0; i < M; ++i) indices[i] = i >= N ? N - 1 : i;
  return a.template swiz<M>(indices);
}

template<int M> 
inline
ShGeneric<M, SH_DOUBLE> fillcast(double a)
{
  return fillcast<M>(ShAttrib<1, SH_CONST, SH_DOUBLE>(a));
}

template<int M, int N, ShValueType V1, ShValueType V2> 
inline
ShGeneric<M+N, CV1V2> join(const ShGeneric<M, V1>& a, const ShGeneric<N, V2>& b)
{
  int indices[M+N];
  for(int i = 0; i < M+N; ++i) indices[i] = i; 
  ShAttrib<M+N, SH_TEMP, CV1V2> result;
  result.template swiz<M>(indices) = a;
  result.template swiz<N>(indices + M) = b;
  return result;
}

template<int N, ShValueType V>
inline
void discard(const ShGeneric<N, V>& c)
{
  shKIL(c);
}

template<int N, ShValueType V>
inline
void kill(const ShGeneric<N, V>& c)
{
  discard(c);
}

template<typename T>
ShProgram freeze(const ShProgram& p,
                 const T& uniform)
{
  return (p >> uniform) << (T::ConstType)(uniform);
}

}

#endif
