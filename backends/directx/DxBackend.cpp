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
#include "DxBackend.hpp"
#include "ShDebug.hpp"

#ifdef WIN32

#endif

namespace shdx {

DxBackend::DxBackend(CodeStrategy* code, TextureStrategy* textures,
                     StreamStrategy* streams)
  : m_curContext(0), m_pD3DDevice(NULL)
{
  m_contexts.push_back(Context(code, textures, streams));
}

SH::ShBackendCodePtr
DxBackend::generateCode(const std::string& target,
                        const SH::ShProgramNodeCPtr& shader)
{
  // Pass the Direct3D device to the context
  m_contexts[m_curContext].code->setD3DDevice(m_pD3DDevice);
  return m_contexts[m_curContext].code->generate(target, shader,
                                                 m_contexts[m_curContext].textures);
}

void
DxBackend::execute(const SH::ShProgramNodeCPtr& program,
                   SH::ShStream& dest)
{
  // TODO: error otherwise.
  if (m_contexts[m_curContext].streams) {
    m_contexts[m_curContext].streams->execute(program, dest);
  }
}

int DxBackend::newContext()
{
  int context_num = m_contexts.size();
  const Context& c = m_contexts[0];
  m_contexts.push_back(Context(c.code->create(context_num),
                               c.textures->create(context_num),
                               c.streams->create(context_num)));

  setContext(context_num);
  return context_num;
}

int DxBackend::context() const
{
  return m_curContext;
}

void DxBackend::setContext(int context)
{
  SH_DEBUG_ASSERT(0 <= context && context < m_contexts.size());
  m_curContext = context;
}

void DxBackend::destroyContext()
{
  if (m_curContext == 0) return;

  Context& c = m_contexts[m_curContext];
  delete c.code; c.code = 0;
  delete c.textures; c.textures = 0;
  delete c.streams; c.streams = 0;
  setContext(0);
}

void DxBackend::setBackendData(void *ptr)
{
	m_pD3DDevice = (LPDIRECT3DDEVICE9)ptr;
}

}
