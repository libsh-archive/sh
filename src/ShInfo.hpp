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
#ifndef SHINFO_HPP
#define SHINFO_HPP

#include <list>
#include <string>
#include "ShDllExport.hpp"

namespace SH {

/** Dummy class representing additional information that can be attached
 * to certain Sh objects for internal usage. 
 */
class 
SH_DLLEXPORT
ShInfo {
public:
  virtual ~ShInfo();
  virtual ShInfo* clone() const = 0;
  
protected:
  ShInfo();
};

/** A simple Info class for attaching string comments to objects */
class
SH_DLLEXPORT
ShInfoComment: public ShInfo {
public:
  ShInfoComment(const std::string& comment); 
  ShInfo* ShInfoComment::clone() const;

  std::string comment; 
};

/** A holder for information 
 *
 * If a subclass declares a user-defined copy-assignment operation (operator=),
 * it must call this operator=.
 */
class
SH_DLLEXPORT ShInfoHolder {
public:
  typedef std::list<ShInfo*> InfoList;
  InfoList info;

  ShInfoHolder();

  // Clones the info list from other 
  ShInfoHolder(const ShInfoHolder &other);

  // Makes this list equal to a clone of other's list
  ShInfoHolder& operator=(const ShInfoHolder &other);

  ~ShInfoHolder();

  // Return the first entry in info whose type matches T, or 0 if no
  // such entry exists.
  // @{
  template<typename T>
  T* get_info();
  template<typename T>
  const T* get_info() const;
  // @}

  // Delete and remove all info entries matching the given type.
  template<typename T>
  void destroy_info();

  // Add the given statement information to the end of the info list.
  void add_info(ShInfo* new_info);

  // Remove the given statement information from the list.
  // Does not delete it, so be careful!
  void remove_info(ShInfo* old_info);

};

} // namespace SH

#include "ShInfoImpl.hpp"

#endif
