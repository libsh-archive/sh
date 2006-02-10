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
#ifndef GLSLSET_HPP
#define GLSLSET_HPP

#include "ShBackend.hpp"

namespace shgl {

class GlslCode;

class GlslSet : public SH::ShBackendSet {
public:
  GlslSet();
  GlslSet(const SH::ShPointer<GlslCode>& code);
  GlslSet(const SH::ShProgramSet& s);
  GlslSet(const GlslSet& s);
  ~GlslSet();

  GlslSet& operator=(const GlslSet& s);
  
  void link();
  void bind();
  void unbind();

  bool empty() const;
  bool bound() const;
  
  void attach(const SH::ShPointer<GlslCode>& code);
  void detach(const SH::ShPointer<GlslCode>& code);
  void replace(const SH::ShPointer<GlslCode>& code);

  // Currently bound set
  static GlslSet* current() { return m_current; }
  
private:
  SH::ShPointer<GlslCode> m_shaders[2];
  GLhandleARB m_arb_program;

  bool m_linked;

  static GlslSet* m_current;
};

typedef SH::ShPointer<GlslSet> GlslSetPtr;
typedef SH::ShPointer<const GlslSet> GlslSetCPtr;

}
#endif
