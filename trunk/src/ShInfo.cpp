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

#include "ShDebug.hpp"
#include "ShInfo.hpp"

namespace SH {

ShInfo::~ShInfo()
{
}

ShInfo::ShInfo() 
{
}

ShInfoComment::ShInfoComment(const std::string& comment) 
  : comment(comment)
{}

ShInfo* ShInfoComment::clone() const
{
  return new ShInfoComment(*this);
}


ShInfoHolder::ShInfoHolder()
{}

ShInfoHolder::~ShInfoHolder()
{
  erase_all();
}

ShInfoHolder::ShInfoHolder(const ShInfoHolder &other)
{
  for(InfoList::const_iterator I = other.info.begin(); I != other.info.end(); ++I) {
    info.push_back((*I)->clone());
  }
}

// Makes this list equal to a clone of other's list
ShInfoHolder& ShInfoHolder::operator=(const ShInfoHolder &other)
{
  if(this == &other) return *this;
  erase_all();
  for(InfoList::const_iterator I = other.info.begin(); I != other.info.end(); ++I) {
    info.push_back((*I)->clone());
  }
  return *this;
}

void ShInfoHolder::add_info(ShInfo* new_info)
{
  info.push_back(new_info);
}

void ShInfoHolder::remove_info(ShInfo* old_info)
{
  info.remove(old_info);
}

void ShInfoHolder::erase_all()
{
  for(InfoList::const_iterator I = info.begin(); I != info.end(); ++I) {
    delete *I;
  }
  info.clear();
}

} // namespace SH
