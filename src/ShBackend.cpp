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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef WIN32
#include <ltdl.h>
#endif

#include <algorithm>

#include "ShBackend.hpp"
#include "ShProgram.hpp"
#include "ShDebug.hpp"
#include "ShBackend.hpp"


namespace SH {

ShBackend::ShBackendList* ShBackend::m_backends = 0;
bool ShBackend::m_doneInit = false;

ShBackendCode::~ShBackendCode()
{
}

ShBackend::ShBackend()
{
  init();
  m_backends->push_back(this);
}

void ShBackend::render(ShVertexArray& array){
SH_DEBUG_ERROR("render is not available in this backend");
}

void ShBackend::render_planar(ShVertexArray& array){
SH_DEBUG_ERROR("render is not available in this backend");
}

ShBackend::~ShBackend()
{
  m_backends->erase(std::remove(begin(), end(), ShBackendPtr(this)), end());
}

ShBackend::ShBackendList::iterator ShBackend::begin()
{
  init();
  return m_backends->begin();
}

ShBackend::ShBackendList::iterator ShBackend::end()
{
  init();
  if (!m_backends) m_backends = new ShBackendList();
  return m_backends->end();
}

ShRefCount<ShBackend> ShBackend::lookup(const std::string& name)
{
  init();
  
  for (ShBackendList::iterator I = begin(); I != end(); ++I) {
    if ((*I)->name() == name)
        {
            (*I)->init2();
            return *I;
        }
  }
#ifndef WIN32
  std::string libname(SH_INSTALL_PREFIX);
  libname += "/lib/sh/libsh" + name + ".so";
  
  lt_dlhandle handle = lt_dlopenext(libname.c_str());

  if (!handle) {
    SH_DEBUG_ERROR("Could not open " << libname << ": " << lt_dlerror());
    return 0;
  }

  for (ShBackendList::iterator I = begin(); I != end(); ++I) {
    if ((*I)->name() == name)
        {
            (*I)->init2();
            return *I;
        }
  }

  SH_DEBUG_ERROR("Could not find " << name << "backend");

  return 0;
#else
  return 0;
#endif
}


void ShBackend::setUberbufferData(SH::ShUberbufferPtr ub, int xoffset, int yoffset,
               int width, int height, const float *data ) {
  printf("no uberbuffer support for this backend...\n");
}

void ShBackend::getUberbufferData(const SH::ShUberbuffer *ub, int xoffset, int yoffset,
                int width, int height, float *data ) {
  printf("no uberbuffer support for this backend...\n");
}

void ShBackend::copyUberbufferData(SH::ShUberbufferPtr dest, SH::ShUberbufferPtr src ) {
  printf("no uberbuffer support for this backend...\n");
}

void ShBackend::deleteFramebuffer(const ShFramebuffer *fb) {
  printf("no framebuffer support for this backend...\n");
}

void ShBackend::deleteUberbuffer(const ShUberbuffer *ub) {
  printf("no uberbuffer support for this backend...\n");
}

void ShBackend::init2(void)
{
  printf("no init for this backend...\n");
}

void ShBackend::clearFrameBuffer(){
   printf("clearFrameBuffer not supported for this backend...\n");
}

void ShBackend::init()
{
  if (m_doneInit) return;
  SH_DEBUG_PRINT("Initializing backend system");
  
  m_backends = new ShBackendList();

#ifndef WIN32
  if (lt_dlinit()) {
    SH_DEBUG_ERROR("Error initializing ltdl: " << lt_dlerror());
  }

  std::string searchpath(SH_INSTALL_PREFIX);
  searchpath += "/lib/sh";

  if (lt_dladdsearchdir(searchpath.c_str())) {
    SH_DEBUG_ERROR("Could not add " + searchpath + " to search dir: " << lt_dlerror());
  }
#endif

  m_doneInit = true;
}

}
