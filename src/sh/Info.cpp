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

#include "Debug.hpp"
#include "Info.hpp"

namespace SH {

Info::~Info()
{
}

Info::Info() 
{
}

InfoComment::InfoComment(const std::string& comment) 
  : comment(comment)
{}

Info* InfoComment::clone() const
{
  return new InfoComment(*this);
}


InfoHolder::InfoHolder()
{}

InfoHolder::~InfoHolder()
{
  erase_all();
}

InfoHolder::InfoHolder(const InfoHolder &other)
{
  for(InfoList::const_iterator I = other.info.begin(); I != other.info.end(); ++I) {
    info.push_back((*I)->clone());
  }
}

// Makes this list equal to a clone of other's list
InfoHolder& InfoHolder::operator=(const InfoHolder &other)
{
  if(this == &other) return *this;
  erase_all();
  for(InfoList::const_iterator I = other.info.begin(); I != other.info.end(); ++I) {
    info.push_back((*I)->clone());
  }
  return *this;
}

void InfoHolder::add_info(Info* new_info)
{
  info.push_back(new_info);
}

void InfoHolder::remove_info(Info* old_info)
{
  info.remove(old_info);
}

void InfoHolder::erase_all()
{
  for(InfoList::const_iterator I = info.begin(); I != info.end(); ++I) {
    delete *I;
  }
  info.clear();
}

} // namespace SH
