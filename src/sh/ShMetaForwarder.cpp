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

std::string ShMetaForwarder::meta(const std::string& key) const
{
  return m_meta->meta(key);
}

void ShMetaForwarder::meta(const std::string& key, const std::string& value)
{
  m_meta->meta(key, value);
}

std::map<std::string, std::string>::const_iterator ShMetaForwarder::begin_meta() const
{
  return m_meta->begin_meta();
}

std::map<std::string, std::string>::const_iterator ShMetaForwarder::end_meta() const
{
  return m_meta->end_meta();
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
