// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
  
  void attach(const SH::ShPointer<GlslCode>& code);
  void detach(const SH::ShPointer<GlslCode>& code);
  void replace(const SH::ShPointer<GlslCode>& code);

  // Currently bound set
  static GlslSet* current() { return m_current; }
  
private:
  SH::ShPointer<GlslCode> m_shaders[2];
  GLhandleARB m_arb_program;

  bool m_linked, m_bound;

  static GlslSet* m_current;
};

typedef SH::ShPointer<GlslSet> GlslSetPtr;
typedef SH::ShPointer<const GlslSet> GlslSetCPtr;

}
#endif
