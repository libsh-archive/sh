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
#include "GlslCode.hpp"
#include <iostream>

#define DEBUG_GLSL_BACKEND

namespace shgl {

using namespace SH;
using namespace std;

GlslSet::GlslSet()
  : m_linked(false)
{
  GL_CHECK_ERROR(m_arb_program = glCreateProgramObjectARB());
  if (!m_arb_program) {
    error(BackendException("Cannot create a glsl program object. Is glsl supported by your GPU/drivers ?"));
  }

  m_shaders[0] = 0;
  m_shaders[1] = 0;
}

GlslSet::GlslSet(const SH::Pointer<GlslCode>& code)
  : m_linked(false)
{
  GL_CHECK_ERROR(m_arb_program = glCreateProgramObjectARB());
  if (!m_arb_program) {
    error(BackendException("Cannot create a glsl program object. Is glsl supported by your GPU/drivers ?"));
  }

  m_shaders[0] = 0;
  m_shaders[1] = 0;
  attach(code);
}

GlslSet::GlslSet(const SH::ProgramSet& s)
  : m_linked(false)
{
  GL_CHECK_ERROR(m_arb_program = glCreateProgramObjectARB());
  if (!m_arb_program) {
    error(BackendException("Cannot create a glsl program object. Is glsl supported by your GPU/drivers ?"));
  }

  m_shaders[0] = 0;
  m_shaders[1] = 0;
  for (ProgramSet::const_iterator I = s.begin(); I != s.end(); ++I) {
    // TODO: use the glsl backend
    GlslCodePtr code = shref_dynamic_cast<GlslCode>((*I)->code());

    // TODO: use error()
    DEBUG_ASSERT(code);
    DEBUG_ASSERT(!m_shaders[code->glsl_unit()]);

    attach(code);
  }
}

GlslSet::GlslSet(const GlslSet& other)
  : m_linked(false)
{
  GL_CHECK_ERROR(m_arb_program = glCreateProgramObjectARB());
  if (!m_arb_program) {
    error(BackendException("Cannot create a glsl program object. Is glsl supported by your GPU/drivers ?"));
  }

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
  GL_CHECK_ERROR(glDeleteObjectARB(m_arb_program));
  m_arb_program = 0;
}

void GlslSet::attach(const SH::Pointer<GlslCode>& code)
{
  DEBUG_ASSERT(m_arb_program);

  if (!code) return;
  if (m_shaders[code->glsl_unit()] == code) return;

  // Unbind old shader
  unbind();
  if (m_shaders[code->glsl_unit()]) {
    GL_CHECK_ERROR(glDetachObjectARB(m_arb_program, m_shaders[code->glsl_unit()]->glsl_shader()));
  }

  // Bind new shader
  GL_CHECK_ERROR(glAttachObjectARB(m_arb_program, code->glsl_shader()));
  m_shaders[code->glsl_unit()] = code;
  code->bind_generic_attributes(m_arb_program);

  m_linked = false; // will need to relink
}

void GlslSet::detach(const SH::Pointer<GlslCode>& code)
{
  if (!code) return;
  if (m_shaders[code->glsl_unit()] != code) return;

  unbind();

  DEBUG_ASSERT(m_arb_program);
  GL_CHECK_ERROR(glDetachObjectARB(m_arb_program, code->glsl_shader()));
  m_shaders[code->glsl_unit()] = 0;

  // Need to relink
  m_linked = false;
}

void GlslSet::replace(const SH::Pointer<GlslCode>& code)
{
  DEBUG_ASSERT(code);
  if (m_shaders[code->glsl_unit()] == code && !m_shaders[1 - code->glsl_unit()]) return;

  detach(m_shaders[0]);
  detach(m_shaders[1]);
  attach(code);
}


void GlslSet::link()
{
  DEBUG_ASSERT(m_arb_program);

  GL_CHECK_ERROR(glLinkProgramARB(m_arb_program));

  // Check linking
  GLint linked;
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

bool GlslSet::bound() const
{
  // Query GL to see if we are bound
  GLhandleARB currently_bound = 0;
  GL_CHECK_ERROR(currently_bound = glGetHandleARB(GL_PROGRAM_OBJECT_ARB));
  return (currently_bound == m_arb_program);
}

void GlslSet::bind()
{
  if (!m_linked) link();

  if (current() && current() != this) current()->unbind();
  
  GL_CHECK_ERROR(glUseProgramObjectARB(m_arb_program));

#ifdef DEBUG_GLSL_BACKEND
  // This could be slow, it should not be enabled in release code
  glValidateProgramARB(m_arb_program);
  GLint validated;
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
}

void GlslSet::unbind()
{
  if (!bound()) return;

  GL_CHECK_ERROR(glUseProgramObjectARB(0));

  for (int i = 0; i < 2; i++) {
    if (!m_shaders[i]) continue;
    m_shaders[i]->set_bound(0);
  }
  m_current = 0;
}

}
