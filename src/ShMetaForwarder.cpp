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
#include "ShMetaForwarder.hpp"
#include "ShMeta.hpp"

namespace SH {

ShMetaForwarder::ShMetaForwarder(ShMeta* meta)
  : m_meta(meta)
{
}

std::string ShMetaForwarder::name() const
{
  return m_meta->name();
}

void ShMetaForwarder::name(const std::string& n)
{
  m_meta->name(n);
}

bool ShMetaForwarder::has_name() const
{
  return m_meta->has_name();
}

bool ShMetaForwarder::internal() const
{
  return m_meta->internal();
}

void ShMetaForwarder::internal(bool i)
{
  m_meta->internal(i);
}

std::string ShMetaForwarder::title() const
{
  return m_meta->title();
}

void ShMetaForwarder::title(const std::string& t)
{
  m_meta->title(t);
}

std::string ShMetaForwarder::description() const
{
  return m_meta->description();
}

void ShMetaForwarder::description(const std::string& d)
{
  m_meta->description(d);
}

std::string ShMetaForwarder::meta(std::string key) const
{
  return m_meta->meta(key);
}

void ShMetaForwarder::meta(std::string key, std::string value)
{
  m_meta->meta(key, value);
}

ShMeta* ShMetaForwarder::real_meta()
{
  return m_meta;
}

void ShMetaForwarder::real_meta(ShMeta* m)
{
  m_meta = m;
}

}
