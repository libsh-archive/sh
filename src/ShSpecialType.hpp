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
#ifndef SHSPECIALTYPE_HPP
#define SHSPECIALTYPE_HPP

#include "ShAttrib.hpp"

/** \file ShSpecialType.hpp
 * \brief Macros for declaring and implementing special types which
 * are really just wrappers around ShAttrib.
 */ 

/** \def SH_SPECIAL_TYPE(SH_TYPE_NAME, SH_COMMENT_NAME, \
 *                       SH_TYPE_ENUM) \
 * \brief Declare an ShAttrib-wrapping type called SH_TYPE_NAME.
 * @param SH_COMMENT_NAME The name used in commenting the type.
 */

/** \def SH_SPECIAL_TYPE_TYPEDEFS(SH_TYPE_NAME_NO_SH) \
 * \brief Declare typedefs for typical uses of a Special Type.
 * @param SH_TYPE_NAME_NO_SH The name of the type without the leading "Sh".
 */

/** \def SH_SPECIAL_TYPE_IMPL(SH_TYPE_NAME, SH_COMMENT_NAME, SH_TYPE_PARENT = ShAttrib)
 * \brief Generate code implementing the member functions of a given
 * Special Type.
 * @param SH_TYPE_NAME The type for which the member function definitions should
 * be generated.
 */

#define SH_SPECIAL_TYPE(SH_TYPE_NAME, SH_COMMENT_NAME, SH_TYPE_ENUM) \
  SH_SPECIAL_TYPE_PARENT(SH_TYPE_NAME, SH_COMMENT_NAME, ShAttrib, SH_TYPE_ENUM)

#define SH_SPECIAL_TYPE_PARENT(SH_TYPE_NAME, SH_COMMENT_NAME, SH_TYPE_PARENT, SH_TYPE_ENUM) \
template<int N, int Kind, typename T=float, bool Swizzled=false> \
class SH_TYPE_NAME : public SH_TYPE_PARENT<N, Kind, T, Swizzled> { \
public: \
  SH_TYPE_NAME(); \
   \
  SH_TYPE_NAME(T); \
  SH_TYPE_NAME(T, T); \
  SH_TYPE_NAME(T, T, T); \
  SH_TYPE_NAME(T, T, T, T); \
 \
  SH_TYPE_NAME(const ShVariableN<N, T>& other); \
  SH_TYPE_NAME(const SH_TYPE_NAME<N, Kind, T, Swizzled>& other); \
  SH_TYPE_NAME(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg); \
 \
  ~SH_TYPE_NAME(); \
 \
 \
  SH_TYPE_NAME& operator=(const ShVariableN<N, T>& other); \
  SH_TYPE_NAME& operator=(const SH_TYPE_NAME<N, Kind, T, Swizzled>& other); \
 \
  SH_TYPE_NAME& operator+=(const ShVariableN<N, T>& right); \
  SH_TYPE_NAME& operator-=(const ShVariableN<N, T>& right); \
  template<typename T2> SH_TYPE_NAME& operator*=(T2 right); \
  template<typename T2> SH_TYPE_NAME& operator/=(T2 right); \
\
  SH_TYPE_NAME& operator+=(const ShConstant<N, T>& right); \
  SH_TYPE_NAME& operator-=(const ShConstant<N, T>& right); \
 \
  SH_TYPE_NAME<N, Kind, T, true> operator()() const; \
  SH_TYPE_NAME<1, Kind, T, true> operator()(int) const; \
  SH_TYPE_NAME<2, Kind, T, true> operator()(int, int) const; \
  SH_TYPE_NAME<3, Kind, T, true> operator()(int, int, int) const; \
  SH_TYPE_NAME<4, Kind, T, true> operator()(int, int, int, int) const; \
  SH_TYPE_NAME<1, Kind, T, true> operator[](int) const; \
  \
  template<int N2> \
  SH_TYPE_NAME<N2, Kind, T, true> swiz(int indices[]) const; \
 \
  SH_TYPE_NAME<N, Kind, T, Swizzled> operator-() const; \
  typedef T ValueType; \
  static const int typesize = N; \
  static const int typekind = Kind; \
  static const ShVariableSpecialType special_type = SH_TYPE_ENUM; \
 \
  typedef SH_TYPE_NAME<N, SH_VAR_INPUT, T> InputType; \
  typedef SH_TYPE_NAME<N, SH_VAR_OUTPUT, T> OutputType; \
  typedef SH_TYPE_NAME<N, SH_VAR_TEMP, T> TempType; \
 \
private: \
  typedef SH_TYPE_PARENT<N, Kind, float, Swizzled> ParentType; \
}

#define SH_SPECIAL_TYPE_TYPEDEF(SH_TYPE_NAME_NO_SH, SH_TYPE_SIZE) \
typedef Sh ## SH_TYPE_NAME_NO_SH < SH_TYPE_SIZE, SH_VAR_INPUT, float> ShInput ## SH_TYPE_NAME_NO_SH ## SH_TYPE_SIZE ## f; \
typedef Sh ## SH_TYPE_NAME_NO_SH < SH_TYPE_SIZE, SH_VAR_OUTPUT, float> ShOutput ## SH_TYPE_NAME_NO_SH ## SH_TYPE_SIZE ## f; \
typedef Sh ## SH_TYPE_NAME_NO_SH < SH_TYPE_SIZE, SH_VAR_TEMP, float> Sh ## SH_TYPE_NAME_NO_SH ## SH_TYPE_SIZE ## f; \

#define SH_SPECIAL_TYPE_TYPEDEFS(SH_TYPE_NAME_NO_SH) \
SH_SPECIAL_TYPE_TYPEDEF(SH_TYPE_NAME_NO_SH, 1) \
SH_SPECIAL_TYPE_TYPEDEF(SH_TYPE_NAME_NO_SH, 2) \
SH_SPECIAL_TYPE_TYPEDEF(SH_TYPE_NAME_NO_SH, 3) \
SH_SPECIAL_TYPE_TYPEDEF(SH_TYPE_NAME_NO_SH, 4) \

#define SH_SPECIAL_TYPE_IMPL(SH_TYPE_NAME, SH_TYPE_ENUM) \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::SH_TYPE_NAME() \
{ \
  m_node->specialType(SH_TYPE_ENUM); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::SH_TYPE_NAME(T t0) \
  : ParentType(t0) \
{ \
  m_node->specialType(SH_TYPE_ENUM); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::SH_TYPE_NAME(T t0, T t1) \
  : ParentType(t0, t1) \
{ \
  m_node->specialType(SH_TYPE_ENUM); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::SH_TYPE_NAME(T t0, T t1, T t2) \
  : ParentType(t0, t1, t2) \
{ \
  m_node->specialType(SH_TYPE_ENUM); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::SH_TYPE_NAME(T t0, T t1, T t2, T t3) \
  : ParentType(t0, t1, t2, t3) \
{ \
  m_node->specialType(SH_TYPE_ENUM); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::SH_TYPE_NAME(const ShVariableN<N, T>& other) \
  : ParentType(other) \
{ \
  m_node->specialType(SH_TYPE_ENUM); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::SH_TYPE_NAME(const SH_TYPE_NAME<N, Kind, T, Swizzled>& other) \
  : ParentType(other) \
{ \
  m_node->specialType(SH_TYPE_ENUM); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::SH_TYPE_NAME(const ShVariableNodePtr& node, \
                                         const ShSwizzle& swizzle, bool neg) \
  : ParentType(node, swizzle, neg) \
{ \
  m_node->specialType(SH_TYPE_ENUM); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::~SH_TYPE_NAME() \
{ \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator=(const ShVariableN<N, T>& other) \
{ \
  ParentType::operator=(other); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator=(const SH_TYPE_NAME<N, Kind, T, Swizzled>& other) \
{ \
  ParentType::operator=(other); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator+=(const ShVariableN<N, T>& other) \
{ \
  ParentType::operator+=(other); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator-=(const ShVariableN<N, T>& other) \
{ \
  ParentType::operator-=(other); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
template<typename T2> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator*=(T2 right) \
{ \
  ParentType::operator*=(right); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
template<typename T2> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator/=(T2 right) \
{ \
  ParentType::operator/=(right); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator+=(const ShConstant<N, T>& other) \
{ \
  ParentType::operator+=(other); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator-=(const ShConstant<N, T>& other) \
{ \
  ParentType::operator-=(other); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, true> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator()() const \
{ \
  return SH_TYPE_NAME<N, Kind, T, true>(m_node, m_swizzle, m_neg); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<1, Kind, T, true> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator()(int s0) const \
{ \
  return SH_TYPE_NAME<1, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<1, Kind, T, true> SH_TYPE_NAME<N, Kind, T, Swizzled>::operator[](int i1) const \
{ \
  return SH_TYPE_NAME<1, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1), m_neg); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<2, Kind, T, true> SH_TYPE_NAME<N, Kind, T, Swizzled>::operator()(int i1, int i2) const \
{ \
  return SH_TYPE_NAME<2, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2), m_neg); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<3, Kind, T, true> SH_TYPE_NAME<N, Kind, T, Swizzled>::operator()(int i1, int i2, int i3) const \
{ \
  return SH_TYPE_NAME<3, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2, i3), m_neg); \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<4, Kind, T, true> SH_TYPE_NAME<N, Kind, T, Swizzled>::operator()(int i1, int i2, int i3, int i4) const \
{ \
  return SH_TYPE_NAME<4, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2, i3, i4), m_neg); \
} \
\
template<int N, int Kind, typename T, bool Swizzled>\
template<int N2>\
SH_TYPE_NAME<N2, Kind, T, true> SH_TYPE_NAME<N, Kind, T, Swizzled>::swiz(int indices[]) const\
{\
  return SH_TYPE_NAME<N2, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);\
}\
 \
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator-() const \
{ \
  return SH_TYPE_NAME<N, Kind, T, Swizzled>(m_node, m_swizzle, !m_neg); \
}

#endif
