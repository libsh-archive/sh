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
#include "ShArb.hpp"
#include <sstream>

#include <GL/gl.h>
#include "ShDebug.hpp"

namespace ShArb {

ArbBackend::GlExtensions::GlExtensions()
{
  const GLubyte* exts = glGetString(GL_EXTENSIONS);

  if (!exts) {
    SH_DEBUG_WARN("Cannot get GL_EXTENSIONS!");
    return;
  }
  std::string extstring((const char*)exts);
  std::istringstream extstream(extstring);

  while (!extstream.eof()) {
    std::string extension;
    extstream >> std::ws >> extension;
    if (!extension.empty()) m_extlist.insert(extension);
  }
}

bool ArbBackend::GlExtensions::have(const std::string& name)
{
  return m_extlist.find(name) != m_extlist.end();
}

void ArbBackend::GlExtensions::dump()
{
  std::cerr << "GL Extensions:" << std::endl;
  for (std::set<std::string>::const_iterator I = m_extlist.begin();
       I != m_extlist.end(); ++I) {
    std::cerr << *I << ", ";
  }
  std::cerr << "(end of list)" << std::endl;
}

}
