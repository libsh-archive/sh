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
#ifndef SHINFOIMPL_HPP
#define SHINFOIMPL_HPP

#include "ShInfo.hpp"

namespace SH {

template<typename T>
T* ShInfoHolder::get_info()
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
const T* ShInfoHolder::get_info() const
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
void ShInfoHolder::destroy_info()
{
  for (InfoList::iterator I = info.begin(); I != info.end();) {
    T* item = dynamic_cast<T*>(*I);
    if (item) {
      I = info.erase(I);
    } else {
      ++I;
    }
  }
}

} // namespace SH

#endif
