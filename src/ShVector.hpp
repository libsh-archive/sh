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
#ifndef SHVECTOR_HPP
#define SHVECTOR_HPP

#include "ShSpecialType.hpp"

namespace SH {

/** An N-Vector.
 */

SH_SPECIAL_TYPE_PARENT_OPEN(ShVector, vector, ShAttrib, SH_VECTOR);
ShVector(); 
ShVector(T); 
ShVector(T, T); 
ShVector(T, T, T); 
ShVector(T, T, T, T); 
ShVector(const ShVariableN<N, T>& other);
template<int Kind2, bool Swizzled2>
ShVector(const ShVector<N, Kind2, T, Swizzled2>& other); 
ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg); 
~ShVector(); 

ShVector& operator=(const ShVariableN<N, T>& other); 
template<int Kind2, bool Swizzled2>
ShVector& operator=(const ShVector<N, Kind2, T, Swizzled2>& other); 

ShVector& operator+=(const ShVariableN<N, T>& right); 
ShVector& operator-=(const ShVariableN<N, T>& right); 
template<typename T2> ShVector& operator*=(T2 right); 
template<typename T2> ShVector& operator/=(T2 right); 

ShVector& operator+=(const ShConstant<N, T>& right); 
ShVector& operator-=(const ShConstant<N, T>& right); 
                  
SH_SPECIAL_TYPE_PARENT_CONST_METHODS(ShVector, vector, ShAttrib, SH_VECTOR);
void setUnit(bool flag);
bool isUnit() const;
SH_SPECIAL_TYPE_PARENT_MEMBERS(ShVector, vector, ShAttrib, SH_VECTOR);
bool m_isunit;
SH_SPECIAL_TYPE_PARENT_CLOSE(ShVector, vector, ShAttrib, SH_VECTOR);

SH_SPECIAL_TYPE_TYPEDEFS(Vector);

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>::ShVector()
  : m_isunit(false)
  { 
    m_node->specialType(SH_VECTOR); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>::ShVector(T t0) 
  : ParentType(t0), m_isunit(false)
  { 
    m_node->specialType(SH_VECTOR); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>::ShVector(T t0, T t1) 
  : ParentType(t0, t1), m_isunit(false)
  { 
    m_node->specialType(SH_VECTOR); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>::ShVector(T t0, T t1, T t2) 
  : ParentType(t0, t1, t2), m_isunit(false)
  { 
    m_node->specialType(SH_VECTOR); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>::ShVector(T t0, T t1, T t2, T t3) 
  : ParentType(t0, t1, t2, t3), m_isunit(false)
  { 
    m_node->specialType(SH_VECTOR); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>::ShVector(const ShVariableN<N, T>& other) 
  : ParentType(other), m_isunit(false)
  { 
    m_node->specialType(SH_VECTOR); 
  } 

template<int N, int Kind, typename T, bool Swizzled>
template<int Kind2, bool Swizzled2>
  ShVector<N, Kind, T, Swizzled>::ShVector(const ShVector<N, Kind2, T, Swizzled2>& other) 
  : ParentType(other), m_isunit(other.isUnit())
  { 
    m_node->specialType(SH_VECTOR); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>::ShVector(const ShVariableNodePtr& node, 
      const ShSwizzle& swizzle, bool neg) 
  : ParentType(node, swizzle, neg), m_isunit(false)
  { 
    m_node->specialType(SH_VECTOR); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>::~ShVector() 
  { 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  void ShVector<N, Kind, T, Swizzled>::setUnit(bool flag)
  {
    m_isunit = flag;
  }

template<int N, int Kind, typename T, bool Swizzled> 
  bool ShVector<N, Kind, T, Swizzled>::isUnit() const
  {
    return m_isunit;
  }

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>& 
  ShVector<N, Kind, T, Swizzled>::operator=(const ShVariableN<N, T>& other) 
  { 
    ParentType::operator=(other); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
template<int Kind2, bool Swizzled2> 
  ShVector<N, Kind, T, Swizzled>& 
  ShVector<N, Kind, T, Swizzled>::operator=(const ShVector<N, Kind2, T, Swizzled2>& other) 
  {
    ParentType::operator=(other); 
    setUnit(other.isUnit());
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>& 
  ShVector<N, Kind, T, Swizzled>::operator+=(const ShVariableN<N, T>& other) 
  { 
    ParentType::operator+=(other); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>& 
  ShVector<N, Kind, T, Swizzled>::operator-=(const ShVariableN<N, T>& other) 
  { 
    ParentType::operator-=(other); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  template<typename T2> 
  ShVector<N, Kind, T, Swizzled>& 
  ShVector<N, Kind, T, Swizzled>::operator*=(T2 right) 
  { 
    ParentType::operator*=(right); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  template<typename T2> 
  ShVector<N, Kind, T, Swizzled>& 
  ShVector<N, Kind, T, Swizzled>::operator/=(T2 right) 
  { 
    ParentType::operator/=(right); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>& 
  ShVector<N, Kind, T, Swizzled>::operator+=(const ShConstant<N, T>& other) 
  { 
    ParentType::operator+=(other); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShVector<N, Kind, T, Swizzled>& 
  ShVector<N, Kind, T, Swizzled>::operator-=(const ShConstant<N, T>& other) 
  { 
    ParentType::operator-=(other); 
    setUnit(false);
    return *this; 
  } 
SH_SPECIAL_TYPE_IMPL_CONST_METHODS(ShVector, SH_VECTOR);
}

#endif
