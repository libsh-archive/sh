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
