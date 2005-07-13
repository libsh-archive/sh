// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SH_METAIMPL_HPP
#define SH_METAIMPL_HPP

#include "ShMeta.hpp"

namespace SH {

inline ShMeta::ShMeta(const ShMeta &other)
  : m_meta(0)
{
  *this = other;
}

inline const ShMeta & ShMeta::operator=(const ShMeta &other)
{
  if(other.m_meta) {
    m_meta = new MetaMap();
    *m_meta = *other.m_meta;
  }
  else {
    m_meta = 0;
  }
  return *this;
}

inline ShMeta::~ShMeta()
{
  delete m_meta;
}

inline std::string ShMeta::name() const
{
  return meta("n"); 
}

inline void ShMeta::name(const std::string& n)
{
  meta("n", n);
}

inline bool ShMeta::has_name() const
{
  return !meta("n").empty(); 
}

inline bool ShMeta::internal() const
{
  return !meta("i").empty(); 
}

inline void ShMeta::internal(bool i)
{
  meta("i", i ? "1" : "");
}

inline std::string ShMeta::title() const
{
  return meta("t");
}

inline void ShMeta::title(const std::string& t)
{
  meta("t", t);
}

inline std::string ShMeta::description() const
{
  return meta("d");
}

inline void ShMeta::description(const std::string& d)
{
  meta("d", d);
}

inline std::string ShMeta::meta(const std::string& key) const
{
  if(!m_meta) return std::string(); 

  MetaMap::const_iterator I = m_meta->find(key);
  if (I == m_meta->end()) return std::string();
  return I->second;
}

inline void ShMeta::meta(const std::string& key, const std::string& value)
{
  if(!m_meta) m_meta = new MetaMap();
  (*m_meta)[key] = value;
}

inline bool ShMeta::has_meta(const std::string& key) const
{
  return m_meta && (m_meta->find(key) != m_meta->end());
}

}

#endif

