#ifndef SHVECTORIMPL_HPP
#define SHVECTORIMPL_HPP

#include "ShVector.hpp"

namespace SH {

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::ShVector()
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::ShVector(T t0)
  : ParentType(t0)
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::ShVector(T t0, T t1)
  : ParentType(t0, t1)
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::ShVector(T t0, T t1, T t2)
  : ParentType(t0, t1, t2)
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::ShVector(T t0, T t1, T t2, T t3)
  : ParentType(t0, t1, t2, t3)
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::ShVector(const ShVariableN<N, T>& other)
  : ParentType(other)
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::ShVector(const ShVector<N, Kind, T, Swizzled>& other)
  : ParentType(other)
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::ShVector(const ShVariableNodePtr& node,
                                         const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>::~ShVector()
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>&
ShVector<N, Kind, T, Swizzled>::operator=(const ShVariableN<N, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>&
ShVector<N, Kind, T, Swizzled>::operator=(const ShVector<N, Kind, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>&
ShVector<N, Kind, T, Swizzled>::operator+=(const ShVariableN<N, T>& other)
{
  ParentType::operator+=(other);
  return *this;
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>&
ShVector<N, Kind, T, Swizzled>::operator-=(const ShVariableN<N, T>& other)
{
  ParentType::operator-=(other);
  return *this;
}

template<int N, int Kind, typename T, bool Swizzled>
template<typename T2>
ShVector<N, Kind, T, Swizzled>&
ShVector<N, Kind, T, Swizzled>::operator*=(const T2& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, int Kind, typename T, bool Swizzled>
template<typename T2>
ShVector<N, Kind, T, Swizzled>&
ShVector<N, Kind, T, Swizzled>::operator/=(const T2& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, true>
ShVector<N, Kind, T, Swizzled>::operator()() const
{
  return ShVector<N, Kind, T, true>(m_node, m_swizzle, m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<1, Kind, T, true>
ShVector<N, Kind, T, Swizzled>::operator()(int s0) const
{
  return ShVector<1, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<1, Kind, T, true> ShVector<N, Kind, T, Swizzled>::operator[](int i1) const
{
  return ShVector<1, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<2, Kind, T, true> ShVector<N, Kind, T, Swizzled>::operator()(int i1, int i2) const
{
  return ShVector<2, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<3, Kind, T, true> ShVector<N, Kind, T, Swizzled>::operator()(int i1, int i2, int i3) const
{
  return ShVector<3, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2, i3), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<4, Kind, T, true> ShVector<N, Kind, T, Swizzled>::operator()(int i1, int i2, int i3, int i4) const
{
  return ShVector<4, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2, i3, i4), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShVector<N, Kind, T, Swizzled>
ShVector<N, Kind, T, Swizzled>::operator-() const
{
  return ShVector<N, Kind, T, Swizzled>(m_node, m_swizzle, !m_neg);
}


}

#endif
