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
#include "GlslCode.hpp"
#include <iostream>

#define SH_DEBUG_GLSL_BACKEND

namespace shgl {

using namespace SH;
using namespace std;

GlslSet::GlslSet()
  : m_arb_program(glCreateProgramObjectARB()),
    m_linked(false), m_bound(false)
{
  m_shaders[0] = 0;
  m_shaders[1] = 0;
}

GlslSet::GlslSet(const SH::ShPointer<GlslCode>& code)
  : m_arb_program(glCreateProgramObjectARB()),
    m_linked(false), m_bound(false)
{
  m_shaders[0] = 0;
  m_shaders[1] = 0;
  attach(code);
}

GlslSet::GlslSet(const SH::ShProgramSet& s)
  : m_arb_program(glCreateProgramObjectARB()),
    m_linked(false), m_bound(false)
{
  m_shaders[0] = 0;
  m_shaders[1] = 0;
  for (ShProgramSet::const_iterator I = s.begin(); I != s.end(); ++I) {
    // TODO: use the glsl backend
    GlslCodePtr code = shref_dynamic_cast<GlslCode>((*I)->code());

    // TODO: use shError()
    SH_DEBUG_ASSERT(code);
    SH_DEBUG_ASSERT(!m_shaders[code->glsl_unit()]);

    attach(code);
  }
}

GlslSet::GlslSet(const GlslSet& other)
  : m_arb_program(glCreateProgramObjectARB()),
    m_linked(false), m_bound(false)
{
  m_shaders[0] = 0;
  m_shaders[1] = 0;
  attach(other.m_shaders[0]);
  attach(other.m_shaders[1]);
}

GlslSet& GlslSet::operator=(const GlslSet& other)
{
  if (&other == this) return *this;

  unbind();

  detach(m_shaders[0]);
  detach(m_shaders[1]);
  attach(other.m_shaders[0]);
  attach(other.m_shaders[1]);

  m_linked = false;

  return *this;
}

GlslSet::~GlslSet()
{
  unbind(); // this is necessary
  SH_GL_CHECK_ERROR(glDeleteObjectARB(m_arb_program));
  m_arb_program = 0;
}

void GlslSet::attach(const SH::ShPointer<GlslCode>& code)
{
  if (!code) return;
  if (m_shaders[code->glsl_unit()] == code) return;

  unbind();
  
  SH_DEBUG_ASSERT(m_arb_program);
  if (m_shaders[code->glsl_unit()]) {
    SH_GL_CHECK_ERROR(glDetachObjectARB(m_arb_program, m_shaders[code->glsl_unit()]->glsl_shader()));
  }
  SH_GL_CHECK_ERROR(glAttachObjectARB(m_arb_program, code->glsl_shader()));
  m_shaders[code->glsl_unit()] = code;

  // Need to relink
  m_linked = false;
}

void GlslSet::detach(const SH::ShPointer<GlslCode>& code)
{
  if (!code) return;
  if (m_shaders[code->glsl_unit()] != code) return;

  unbind();

  SH_DEBUG_ASSERT(m_arb_program);
  SH_GL_CHECK_ERROR(glDetachObjectARB(m_arb_program, code->glsl_shader()));
  m_shaders[code->glsl_unit()] = 0;

  // Need to relink
  m_linked = false;
}

void GlslSet::replace(const SH::ShPointer<GlslCode>& code)
{
  SH_DEBUG_ASSERT(code);
  if (m_shaders[code->glsl_unit()] == code && !m_shaders[1 - code->glsl_unit()]) return;

  detach(m_shaders[0]);
  detach(m_shaders[1]);
  attach(code);
}


void GlslSet::link()
{
  SH_DEBUG_ASSERT(m_arb_program);

  SH_GL_CHECK_ERROR(glLinkProgramARB(m_arb_program));

  // Check linking
  int linked;
  glGetObjectParameterivARB(m_arb_program, GL_OBJECT_LINK_STATUS_ARB, &linked);
  if (linked != GL_TRUE) {
    cout << "Program link status: FAILED" << endl << endl;
    cout << "Program infolog:" << endl;
    print_infolog(m_arb_program);
    cout << "Program code:" << endl;
    print_shader_source(m_arb_program);
    cout << endl;
    return;
  }

  m_linked = true;
}

bool GlslSet::empty() const
{
  if (m_shaders[0]) return false;
  if (m_shaders[1]) return false;
  return true;
}

void GlslSet::bind()
{
  if (m_bound) {
    for (int i = 0; i < 2; i++) {
      if (!m_shaders[i]) continue;
      m_shaders[i]->update();
    }
    return;
  }

  if (!m_linked) link();

  if (current() && current() != this) current()->unbind();
  
  SH_GL_CHECK_ERROR(glUseProgramObjectARB(m_arb_program));

#ifdef SH_DEBUG_GLSL_BACKEND
  // This could be slow, it should not be enabled in release code
  glValidateProgramARB(m_arb_program);
  int validated;
  glGetObjectParameterivARB(m_arb_program, GL_OBJECT_VALIDATE_STATUS_ARB, &validated);
  if (validated != GL_TRUE) {
    cout << "Program validate status: FAILED" << endl;
    cout << "Program infolog:" << endl;
    print_infolog(m_arb_program);
  }
#endif

  for (int i = 0; i < 2; i++) {
    if (!m_shaders[i]) continue;
    GlslCodePtr shader = m_shaders[i];
    shader->set_bound(m_arb_program);

    shader->upload_uniforms();
    shader->bind_textures();
  }

  m_current = this;
  m_bound = true;
}

void GlslSet::unbind()
{
  if (!m_bound) return;

  SH_GL_CHECK_ERROR(glUseProgramObjectARB(0));

  for (int i = 0; i < 2; i++) {
    if (!m_shaders[i]) continue;
    m_shaders[i]->set_bound(0);
  }
  m_current = 0;
  m_bound = false;
}

}
