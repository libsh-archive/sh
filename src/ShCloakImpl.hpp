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
#ifndef SHCLOAK_IMPL_HPP
#define SHCLOAK_IMPL_HPP

#include <algorithm>
#include <sstream>
#include "ShDebug.hpp"
#include "ShCloak.hpp"
#include "ShTypeInfo.hpp"

namespace SH {

template<typename T>
ShDataCloak<T>::ShDataCloak(int N)
  : m_data(N, 0)
{
}

template<typename T>
ShDataCloak<T>::ShDataCloak(int N, const T &value)
  : m_data(N, value)
{
}

template<typename T>
ShDataCloak<T>::ShDataCloak(const ShDataCloak<T> &other)
  : m_data(other.m_data)
{
}

template<typename T>
ShDataCloak<T>::ShDataCloak(const ShDataCloak<T> &other, 
    bool neg, const ShSwizzle &swizzle)
  : m_data(swizzle.size())
{
  for(int i = 0; i < swizzle.size(); ++i) {
    m_data[i] = (neg ? -other[swizzle[i]] : other[swizzle[i]]);
  }
}

/*
template<typename T>
std::string ShDataCloak<T>::typeName() const {
  return ShConcreteTypeInfo<T>::m_name; 
}
*/

template<typename T>
ShDataCloak<T>::ShDataCloak(std::string s)
{
   decode(s);
}

template<typename T>
int ShDataCloak<T>::size() const
{
  return m_data.size();
}

template<typename T>
void ShDataCloak<T>::negate()
{
  transform(m_data.begin(), m_data.end(), m_data.begin(), std::negate<T>());
}

template<typename T>
void ShDataCloak<T>::set(ShCloakCPtr other)
{
  CPtrType castOther = shref_dynamic_cast<const ShDataCloak<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    SH_DEBUG_ASSERT(other->size() == size());
    std::copy(castOther->data().begin(), castOther->data().end(), m_data.begin());
  }
}

template<typename T>
void ShDataCloak<T>::set(ShCloakCPtr other, int index)
{
  CPtrType castOther = shref_dynamic_cast<const ShDataCloak<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    SH_DEBUG_ASSERT(index >= 0 && index < size() && castOther->size() > 0); 
    m_data[index] = (*castOther)[0];
  }
}

template<typename T>
void ShDataCloak<T>::set(ShCloakCPtr other, bool neg, const ShSwizzle &writemask) 
{
  CPtrType castOther = shref_dynamic_cast<const ShDataCloak<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    for(int i = 0; i < writemask.size(); ++i) {
      m_data[writemask[i]] = neg ? -(*castOther)[i] : (*castOther)[i];
    }
  }
}

template<typename T>
ShCloakPtr ShDataCloak<T>::get() const
{
  return new ShDataCloak<T>(*this);
}

template<typename T>
ShCloakPtr ShDataCloak<T>::get(int index) const
{
  return new ShDataCloak<T>(1, m_data[index]);
}

template<typename T>
ShCloakPtr ShDataCloak<T>::get(bool neg, const ShSwizzle &swizzle) const 
{
  return new ShDataCloak<T>(*this, neg, swizzle);
}

template<typename T>
T& ShDataCloak<T>::operator[](int index) 
{
  return m_data[index];
}

template<typename T>
const T& ShDataCloak<T>::operator[](int index) const
{
  return m_data[index];
}

template<typename T>
std::vector<T>& ShDataCloak<T>::data() {
  return m_data;
}

template<typename T>
const std::vector<T>& ShDataCloak<T>::data() const {
  return m_data;
}

template<typename T>
std::string ShDataCloak<T>::encode() const {
  if(size() < 1) return "";

  std::ostringstream out;
  out << m_data.size();
  for(int i = 0; i < (int)m_data.size(); ++i) {
    out << "," << m_data[i];
  }
  return out.str();
}

template<typename T>
void ShDataCloak<T>::decode(std::string value) const {
  m_data.clear();

  std::istringstream in(value);

  int size;
  in >> size;
  m_data.reserve(size);

  T component; 
  for(int i = 0; i < size; ++i) {
    in.ignore(1, ',');
    in >> component;
    m_data.push_back(component);
  }
}

}
#endif
