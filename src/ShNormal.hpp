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
#ifndef SHNORMAL_HPP
#define SHNORMAL_HPP

#include "ShSpecialType.hpp"

namespace SH {

/** A surface normal with N dimensions.
 */

SH_SPECIAL_TYPE_PARENT_OPEN(ShNormal, surface normal, ShAttrib, SH_NORMAL);
ShNormal(); 
ShNormal(T); 
ShNormal(T, T); 
ShNormal(T, T, T); 
ShNormal(T, T, T, T); 
ShNormal(const ShVariableN<N, T>& other);
template<int Kind2, bool Swizzled2>
ShNormal(const ShNormal<N, Kind2, T, Swizzled2>& other); 
ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg); 
~ShNormal(); 

ShNormal& operator=(const ShVariableN<N, T>& other); 
template<int Kind2, bool Swizzled2>
ShNormal& operator=(const ShNormal<N, Kind2, T, Swizzled2>& other); 

ShNormal& operator+=(const ShVariableN<N, T>& right); 
ShNormal& operator-=(const ShVariableN<N, T>& right); 
template<typename T2> ShNormal& operator*=(T2 right); 
template<typename T2> ShNormal& operator/=(T2 right); 

ShNormal& operator+=(const ShConstant<N, T>& right); 
ShNormal& operator-=(const ShConstant<N, T>& right); 
                  
SH_SPECIAL_TYPE_PARENT_CONST_METHODS(ShNormal, surface normal, ShAttrib, SH_NORMAL);
void setUnit(bool flag);
bool isUnit() const;
SH_SPECIAL_TYPE_PARENT_MEMBERS(ShNormal, surface normal, ShAttrib, SH_NORMAL);
bool m_isunit;
SH_SPECIAL_TYPE_PARENT_CLOSE(ShNormal, surface normal, ShAttrib, SH_NORMAL);

SH_SPECIAL_TYPE_TYPEDEFS(Normal);

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>::ShNormal()
  : m_isunit(false)
  { 
    m_node->specialType(SH_NORMAL); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>::ShNormal(T t0) 
  : ParentType(t0), m_isunit(false)
  { 
    m_node->specialType(SH_NORMAL); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>::ShNormal(T t0, T t1) 
  : ParentType(t0, t1), m_isunit(false)
  { 
    m_node->specialType(SH_NORMAL); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>::ShNormal(T t0, T t1, T t2) 
  : ParentType(t0, t1, t2), m_isunit(false)
  { 
    m_node->specialType(SH_NORMAL); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>::ShNormal(T t0, T t1, T t2, T t3) 
  : ParentType(t0, t1, t2, t3), m_isunit(false)
  { 
    m_node->specialType(SH_NORMAL); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>::ShNormal(const ShVariableN<N, T>& other) 
  : ParentType(other), m_isunit(false)
  { 
    m_node->specialType(SH_NORMAL); 
  } 

template<int N, int Kind, typename T, bool Swizzled>
template<int Kind2, bool Swizzled2>
  ShNormal<N, Kind, T, Swizzled>::ShNormal(const ShNormal<N, Kind2, T, Swizzled2>& other) 
  : ParentType(other), m_isunit(other.isUnit())
  { 
    m_node->specialType(SH_NORMAL); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>::ShNormal(const ShVariableNodePtr& node, 
      const ShSwizzle& swizzle, bool neg) 
  : ParentType(node, swizzle, neg), m_isunit(false)
  { 
    m_node->specialType(SH_NORMAL); 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>::~ShNormal() 
  { 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  void ShNormal<N, Kind, T, Swizzled>::setUnit(bool flag)
  {
    m_isunit = flag;
  }

template<int N, int Kind, typename T, bool Swizzled> 
  bool ShNormal<N, Kind, T, Swizzled>::isUnit() const
  {
    return m_isunit;
  }

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>& 
  ShNormal<N, Kind, T, Swizzled>::operator=(const ShVariableN<N, T>& other) 
  { 
    ParentType::operator=(other); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
template<int Kind2, bool Swizzled2> 
  ShNormal<N, Kind, T, Swizzled>& 
  ShNormal<N, Kind, T, Swizzled>::operator=(const ShNormal<N, Kind2, T, Swizzled2>& other) 
  {
    ParentType::operator=(other); 
    setUnit(other.isUnit());
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>& 
  ShNormal<N, Kind, T, Swizzled>::operator+=(const ShVariableN<N, T>& other) 
  { 
    ParentType::operator+=(other); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>& 
  ShNormal<N, Kind, T, Swizzled>::operator-=(const ShVariableN<N, T>& other) 
  { 
    ParentType::operator-=(other); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  template<typename T2> 
  ShNormal<N, Kind, T, Swizzled>& 
  ShNormal<N, Kind, T, Swizzled>::operator*=(T2 right) 
  { 
    ParentType::operator*=(right); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  template<typename T2> 
  ShNormal<N, Kind, T, Swizzled>& 
  ShNormal<N, Kind, T, Swizzled>::operator/=(T2 right) 
  { 
    ParentType::operator/=(right); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>& 
  ShNormal<N, Kind, T, Swizzled>::operator+=(const ShConstant<N, T>& other) 
  { 
    ParentType::operator+=(other); 
    setUnit(false);
    return *this; 
  } 

template<int N, int Kind, typename T, bool Swizzled> 
  ShNormal<N, Kind, T, Swizzled>& 
  ShNormal<N, Kind, T, Swizzled>::operator-=(const ShConstant<N, T>& other) 
  { 
    ParentType::operator-=(other); 
    setUnit(false);
    return *this; 
  } 
SH_SPECIAL_TYPE_IMPL_CONST_METHODS(ShNormal, SH_NORMAL);
}

#endif
