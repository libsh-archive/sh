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
#include "ShMeta.hpp"

namespace SH {

ShMeta::ShMeta()
  : m_has_name(false), m_internal(false)
{
}

ShMeta::~ShMeta()
{
}

std::string ShMeta::name() const
{
  return m_name;
}

void ShMeta::name(const std::string& n)
{
  m_name = n;
  m_has_name = true;
}

bool ShMeta::has_name() const
{
  return m_has_name;
}

bool ShMeta::internal() const
{
  return m_internal;
}

void ShMeta::internal(bool i)
{
  m_internal = i;
}

const std::string& ShMeta::title() const
{
  return m_title;
}

void ShMeta::title(const std::string& t)
{
  m_title = t;
}

const std::string& ShMeta::description() const
{
  return m_description;
}

void ShMeta::description(const std::string& d)
{
  m_description = d;
}

std::string ShMeta::meta(std::string key) const
{
  MetaMap::const_iterator I = m_meta.find(key);
  if (I == m_meta.end()) return std::string();
  return I->second;
}

void ShMeta::meta(std::string key, std::string value)
{
  m_meta[key] = value;
}

}

