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
#ifndef SHMETA_HPP
#define SHMETA_HPP

#include <string>
#include <map>

namespace SH {

class ShMeta {
public:
  ShMeta();
  virtual ~ShMeta();
  
  virtual std::string name() const;
  virtual void name(const std::string& n);
  virtual bool has_name() const;
  
  virtual bool internal() const;
  virtual void internal(bool);

  virtual const std::string& title() const;
  virtual void title(const std::string& t);

  virtual const std::string& description() const;
  virtual void description(const std::string& d);

  virtual std::string meta(std::string key) const;
  virtual void meta(std::string key, std::string value);

private:
  std::string m_name;
  bool m_has_name;

  bool m_internal;
  std::string m_title;
  std::string m_description;

  typedef std::map<std::string, std::string> MetaMap;
  MetaMap m_meta;
};

}

#endif
