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
#ifndef SHINFO_HPP
#define SHINFO_HPP

#include <list>
#include <string>
#include "DllExport.hpp"

namespace SH {

/** Dummy class representing additional information that can be attached
 * to certain Sh objects for internal usage. 
 */
class 
SH_DLLEXPORT
Info {
public:
  virtual ~Info();
  virtual Info* clone() const = 0;
  
protected:
  Info();
};

/** A simple Info class for attaching string comments to objects */
class
SH_DLLEXPORT
InfoComment: public Info {
public:
  InfoComment(const std::string& comment); 
  Info* clone() const;

  std::string comment; 
};

/** A holder for information 
 *
 * If a subclass declares a user-defined copy-assignment operation (operator=),
 * it must call this operator=.
 */
class
SH_DLLEXPORT InfoHolder {
public:

  InfoHolder();

  // Clones the info list from other 
  InfoHolder(const InfoHolder &other);

  // Makes this list equal to a clone of other's list
  InfoHolder& operator=(const InfoHolder &other);

  ~InfoHolder();

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
  void add_info(Info* new_info);

  // Remove the given statement information from the list.
  // Does not delete it, so be careful!
  void remove_info(Info* old_info);

private:
  void erase_all();

  typedef std::list<Info*> InfoList;
  InfoList info;

};

} // namespace SH

#include "InfoImpl.hpp"

#endif
