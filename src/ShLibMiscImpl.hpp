// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

template<int M, int N, typename T> 
ShGeneric<M, T> cast(const ShGeneric<N, T>& a)
{
  int copySize = std::min(M, N);
  ShAttrib<M, SH_TEMP, T> result;

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
ShGeneric<M, double> cast(double a)
{
  return cast<M>(ShAttrib<1, SH_CONST, double>(a));
}

template<int M, int N, typename T> 
ShGeneric<M, T> fillcast(const ShGeneric<N, T>& a)
{
  if( M <= N ) return cast<M>(a);
  int indices[M];
  for(int i = 0; i < M; ++i) indices[i] = i >= N ? N - 1 : i;
  return a.template swiz<M>(indices);
}

template<int M> 
inline
ShGeneric<M, double> fillcast(double a)
{
  return fillcast<M>(ShAttrib<1, SH_CONST, double>(a));
}

template<int M, int N, typename T1, typename T2> 
ShGeneric<M+N, CT1T2> join(const ShGeneric<M, T1>& a, const ShGeneric<N, T2>& b)
{
  int indices[M+N];
  for(int i = 0; i < M+N; ++i) indices[i] = i; 
  ShAttrib<M+N, SH_TEMP, CT1T2> result;
  result.template swiz<M>(indices) = a;
  result.template swiz<N>(indices + M) = b;
  return result;
}

template<int M, int N, int O, typename T1, typename T2, typename T3> 
ShGeneric<M+N+O, CT1T2T3> join(const ShGeneric<M, T1>& a, 
			       const ShGeneric<N, T2> &b, 
			       const ShGeneric<O, T3> &c)
{
  int indices[M+N+O];
  for(int i = 0; i < M+N+O; ++i) indices[i] = i; 
  ShAttrib<M+N+O, SH_TEMP, CT1T2T3> result;
  result.template swiz<M>(indices) = a;
  result.template swiz<N>(indices + M) = b;
  result.template swiz<N>(indices + M + N) = c;
  return result;
}

template<int M, int N, int O, int P, typename T1, typename T2, typename T3, typename T4> 
ShGeneric<M+N+O+P, CT1T2T3T4> join(const ShGeneric<M, T1>& a, 
				   const ShGeneric<N, T2> &b, 
				   const ShGeneric<O, T3> &c, 
				   const ShGeneric<P, T4> &d)
{
  int indices[M+N+O+P];
  for(int i = 0; i < M+N+O+P; ++i) indices[i] = i; 
  ShAttrib<M+N+O+P, SH_TEMP, CT1T2T3T4> result;
  result.template swiz<M>(indices) = a;
  result.template swiz<N>(indices + M) = b;
  result.template swiz<N>(indices + M + N) = c;
  result.template swiz<N>(indices + M + N + O) = d;
  return result;
}

template<int N, typename T>
inline
void discard(const ShGeneric<N, T>& c)
{
  shKIL(c);
}

template<int N, typename T>
inline
void kill(const ShGeneric<N, T>& c)
{
  discard(c);
}

template<int N, typename T> 
ShGeneric<N, T> sort(const ShGeneric<N, T>& a)
{
  ShGeneric<N, T> result(a);
  groupsort<1>(&result);
  return result;
}

template<int S, int N, typename T>
void groupsort(ShGeneric<N, T> v[]) {
  const int NE = (N + 1) / 2; // number of even elements
  const int NO = N / 2; // number of odd elements
  const int NU = NO; // number of components to compare for (2i, 2i+1) comparisons
  const int ND = NE - 1; // number of componnets to compare for (2i, 2i-1) comparisons

  int i, j;
  // hold even/odd temps and condition code for (2i, 2i+1) "up" and (2i, 2i-1) "down" comparisons 
  ShAttrib<NU, SH_TEMP, T> eu, ou, ccu; 
  ShAttrib<ND, SH_TEMP, T> ed, od, ccd; 

  // even and odd swizzle (elms 0..NE-1 are the "even" subsequence, NE..N-1 "odd")
  int eswiz[NE], oswiz[NO]; 
  for(i = 0; i < NE; ++i) eswiz[i] = i;
  for(i = 0; i < NO; ++i) oswiz[i] = NE + i;

  for(i = 0; i < NE; ++i) { 
    // TODO note the interesting syntax (does appear to be C++ standard) 
    // that's required so that the gcc parser does 
    // not crap out on the template swiz code
    //
    // if this doesn't work on other platforms, we may have to
    // rewrite the swiz function

    // compare 2i, 2i+1
    eu = v[0].template swiz<NU>(eswiz);
    ou = v[0].template swiz<NU>(oswiz);
    if (S > 1) ccu = eu < ou; 
    v[0].template swiz<NU>(eswiz) = min(eu, ou); 
    v[0].template swiz<NU>(oswiz) = max(eu, ou); 

    for(j = 1; j < S; ++j) {
      eu = v[j].template swiz<NU>(eswiz);
      ou = v[j].template swiz<NU>(oswiz);
      v[j].template swiz<NU>(eswiz) = cond(ccu, eu, ou); 
      v[j].template swiz<NU>(oswiz) = cond(ccu, ou, eu); 
    }

    // compare 2i, 2i-1
    ed = v[0].template swiz<ND>(eswiz + 1);
    od = v[0].template swiz<ND>(oswiz);
    if (S > 1) ccd = ed > od; 
    v[0].template swiz<ND>(eswiz + 1) = max(ed, od);
    v[0].template swiz<ND>(oswiz) = min(ed, od);

    for(j = 1; j < S; ++j) {
      ed = v[j].template swiz<ND>(eswiz + 1);
      od = v[j].template swiz<ND>(oswiz);
      v[j].template swiz<ND>(eswiz + 1) = cond(ccd, ed, od); 
      v[j].template swiz<ND>(oswiz) = cond(ccd, od, ed); 
    }
  }

  // reswizzle "even" to 0, 2, 4,... "odd" to 1, 3, 5, ..
  int resultEswiz[NE], resultOswiz[NO]; 
  for(i = 0; i < NE; ++i) resultEswiz[i] = i * 2;
  for(i = 0; i < NO; ++i) resultOswiz[i] = i * 2 + 1; 
  for(i = 0; i < S; ++i) {
    ShAttrib<NE, SH_TEMP, T> evens = v[i].template swiz<NE>(eswiz);
    v[i].template swiz<NO>(resultOswiz) = v[i].template swiz<NO>(oswiz);
    v[i].template swiz<NE>(resultEswiz) = evens;
  }
}

template<typename T>
inline
ShProgram freeze(const ShProgram& p,
                 const T& uniform)
{
  return (p >> uniform) << (T::ConstType)(uniform);
}

}

#endif
