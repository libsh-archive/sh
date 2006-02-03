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
#ifndef SHMETA_HPP
#define SHMETA_HPP

#include <string>
#include <map>
#include "ShDllExport.hpp"

namespace SH {

class
SH_DLLEXPORT ShMeta {
public:
  ShMeta()
    : m_meta(0) 
  {
  }

  ShMeta(const ShMeta &other);

  const ShMeta & operator=(const ShMeta &other);

  virtual ~ShMeta();
  
  virtual std::string name() const;
  virtual void name(const std::string& n);
  virtual bool has_name() const;
  
  virtual bool internal() const;
  virtual void internal(bool);

  virtual std::string title() const;
  virtual void title(const std::string& t);

  virtual std::string description() const;
  virtual void description(const std::string& d);

  virtual std::string meta(const std::string& key) const;
  virtual void meta(const std::string& key, const std::string& value);
  virtual bool has_meta(const std::string& key) const;

  typedef std::map<std::string, std::string> MetaMap;

  virtual MetaMap::const_iterator begin_meta() const;
  virtual MetaMap::const_iterator end_meta() const;

private:
  mutable MetaMap *m_meta;
};

}

#include "ShMetaImpl.hpp"

#endif
