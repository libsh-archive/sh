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
#ifndef SHINFOIMPL_HPP
#define SHINFOIMPL_HPP

#include "Info.hpp"

namespace SH {

template<typename T>
T* InfoHolder::get_info()
{
  for (InfoList::iterator I = info.begin(); I != info.end(); ++I) {
    T* item = dynamic_cast<T*>(*I);
    if (item) {
      return item;
    }
  }
  return 0;
}

template<typename T>
const T* InfoHolder::get_info() const
{
  for (InfoList::const_iterator I = info.begin(); I != info.end(); ++I) {
    const T* item = dynamic_cast<const T*>(*I);
    if (item) {
      return item;
    }
  }
  return 0;
}

template<typename T>
void InfoHolder::destroy_info()
{
  for (InfoList::iterator I = info.begin(); I != info.end();) {
    T* item = dynamic_cast<T*>(*I);
    if (item) {
      I = info.erase(I);
      delete item;
    } else {
      ++I;
    }
  }
}

} // namespace SH

#endif
