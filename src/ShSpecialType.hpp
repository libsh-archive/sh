#ifndef SHSPECIALTYPE_HPP
#define SHSPECIALTYPE_HPP

#include "ShAttrib.hpp"

/** \file ShSpecialType.hpp
 * \brief Macros for declaring and implementing special types which
 * are really just wrappers around ShAttrib.
 */ 

/** \def SH_SPECIAL_TYPE(SH_TYPE_NAME, SH_COMMENT_NAME) \
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

#define SH_SPECIAL_TYPE(SH_TYPE_NAME, SH_COMMENT_NAME) \
  SH_SPECIAL_TYPE_PARENT(SH_TYPE_NAME, SH_COMMENT_NAME, ShAttrib)

#define SH_SPECIAL_TYPE_PARENT(SH_TYPE_NAME, SH_COMMENT_NAME, SH_TYPE_PARENT) \
template<int N, int Kind, typename T, bool Swizzled=false> \
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
  template<typename T2> SH_TYPE_NAME& operator*=(const T2& right); \
  template<typename T2> SH_TYPE_NAME& operator/=(const T2& right); \
 \
  SH_TYPE_NAME<N, Kind, T, true> operator()() const; \
  SH_TYPE_NAME<1, Kind, T, true> operator()(int) const; \
  SH_TYPE_NAME<2, Kind, T, true> operator()(int, int) const; \
  SH_TYPE_NAME<3, Kind, T, true> operator()(int, int, int) const; \
  SH_TYPE_NAME<4, Kind, T, true> operator()(int, int, int, int) const; \
  SH_TYPE_NAME<1, Kind, T, true> operator[](int) const; \
 \
  SH_TYPE_NAME<N, Kind, T, Swizzled> operator-() const; \
 \
private: \
  typedef SH_TYPE_PARENT<N, Kind, double, Swizzled> ParentType; \
}

#define SH_SPECIAL_TYPE_TYPEDEF(SH_TYPE_NAME_NO_SH, SH_TYPE_SIZE) \
typedef Sh ## SH_TYPE_NAME_NO_SH < SH_TYPE_SIZE, SH_VAR_INPUT, double> ShInput ## SH_TYPE_NAME_NO_SH ## SH_TYPE_SIZE ## f; \
typedef Sh ## SH_TYPE_NAME_NO_SH < SH_TYPE_SIZE, SH_VAR_OUTPUT, double> ShOutput ## SH_TYPE_NAME_NO_SH ## SH_TYPE_SIZE ## f; \
typedef Sh ## SH_TYPE_NAME_NO_SH < SH_TYPE_SIZE, SH_VAR_TEMP, double> Sh ## SH_TYPE_NAME_NO_SH ## SH_TYPE_SIZE ## f; \

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
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator*=(const T2& right) \
{ \
  ParentType::operator*=(right); \
  return *this; \
} \
 \
template<int N, int Kind, typename T, bool Swizzled> \
template<typename T2> \
SH_TYPE_NAME<N, Kind, T, Swizzled>& \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator/=(const T2& right) \
{ \
  ParentType::operator/=(right); \
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
template<int N, int Kind, typename T, bool Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled> \
SH_TYPE_NAME<N, Kind, T, Swizzled>::operator-() const \
{ \
  return SH_TYPE_NAME<N, Kind, T, Swizzled>(m_node, m_swizzle, !m_neg); \
}

#endif
