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
#ifndef SHMETAFORWARDER_HPP
#define SHMETAFORWARDER_HPP

#include <string>
#include <map>
#include "DllExport.hpp"

namespace SH {

class Meta;

class
SH_DLLEXPORT MetaForwarder {
public:
  MetaForwarder(Meta* meta);
  
  std::string name() const;
  void name(const std::string& n);
  bool has_name() const;
  
  bool internal() const;
  void internal(bool);

  std::string title() const;
  void title(const std::string& t);

  std::string description() const;
  void description(const std::string& d);

  std::string meta(const std::string& key) const;
  void meta(const std::string& key, const std::string& value);

  std::map<std::string, std::string>::const_iterator begin_meta() const;
  std::map<std::string, std::string>::const_iterator end_meta() const;

protected:
  Meta* real_meta();
  void real_meta(Meta*);
  
private:
  Meta* m_meta;
};

}

#endif
