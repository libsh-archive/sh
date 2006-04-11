// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#ifndef SHMETAIMPL_HPP
#define SHMETAIMPL_HPP

#include "Meta.hpp"

namespace SH {

inline Meta::Meta(const Meta &other)
  : m_meta(0)
{
  *this = other;
}

inline const Meta & Meta::operator=(const Meta &other)
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

inline Meta::~Meta()
{
  delete m_meta;
}

inline std::string Meta::name() const
{
  return meta("n"); 
}

inline void Meta::name(const std::string& n)
{
  meta("n", n);
}

inline bool Meta::has_name() const
{
  return !meta("n").empty(); 
}

inline bool Meta::internal() const
{
  return !meta("i").empty(); 
}

inline void Meta::internal(bool i)
{
  meta("i", i ? "1" : "");
}

inline std::string Meta::title() const
{
  return meta("t");
}

inline void Meta::title(const std::string& t)
{
  meta("t", t);
}

inline std::string Meta::description() const
{
  return meta("d");
}

inline void Meta::description(const std::string& d)
{
  meta("d", d);
}

inline std::string Meta::meta(const std::string& key) const
{
  if (!m_meta) return std::string(); 

  MetaMap::const_iterator I = m_meta->find(key);
  if (I == m_meta->end()) return std::string();
  return I->second;
}

inline Meta::MetaMap::const_iterator Meta::begin_meta() const
{
  if (!m_meta) m_meta = new MetaMap();
  return m_meta->begin();
}

inline Meta::MetaMap::const_iterator Meta::end_meta() const
{
  if (!m_meta) m_meta = new MetaMap();
  return m_meta->end();
}

inline void Meta::meta(const std::string& key, const std::string& value)
{
  if(!m_meta) m_meta = new MetaMap();
  (*m_meta)[key] = value;
}

inline bool Meta::has_meta(const std::string& key) const
{
  return m_meta && (m_meta->find(key) != m_meta->end());
}

}

#endif

