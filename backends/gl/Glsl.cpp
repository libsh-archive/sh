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
#include "GlBackend.hpp"
#include "Glsl.hpp"
#include "GlslCode.hpp"
#include <iostream>

namespace shgl {

using namespace SH;
using namespace std;

GlslCodeStrategy::GlslCodeStrategy(void)
{
}

GlslCodeStrategy* GlslCodeStrategy::create(void)
{
  return new GlslCodeStrategy;
}

ShBackendCodePtr GlslCodeStrategy::generate(const std::string& target,
                                           const ShProgramNodeCPtr& shader,
                                           TextureStrategy* texture)
{
  std::string::size_type loc = target.rfind(':');
  std::string unit = (loc == std::string::npos ? target : target.substr(loc + 1));
  GlslCodePtr code = new GlslCode(shader, unit, texture);
  code->generate();
  return code;
}

ShBackendSetPtr GlslCodeStrategy::generate_set(const SH::ShProgramSet& s)
{
  GlslSetPtr glsl_set = new GlslSet(s);
  return glsl_set;
}

bool GlslCodeStrategy::use_default_set() const
{
  return false;
}

void GlslCodeStrategy::unbind_all_programs()
{
  if (GlslSet::current()) GlslSet::current()->unbind();
}

bool GlslCodeStrategy::use_default_unbind_all() const
{
  return false;
}


unsigned int glslTarget(const std::string& unit)
{
  if (unit == "vertex") return GL_VERTEX_PROGRAM_ARB;
  if (unit == "fragment") return GL_FRAGMENT_PROGRAM_ARB;  
  return 0;
}

GlslException::GlslException(const std::string& message)
  : ShBackendException(std::string("GLSL error: ") + message)
{
}

void print_infolog(GLhandleARB obj, std::ostream& out)
{
  GLint infolog_len;
  glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infolog_len);
  
  if (infolog_len > 0) {
    char* infolog = (char*)malloc(infolog_len);
    GLint nb_chars;
    glGetInfoLogARB(obj, infolog_len, &nb_chars, infolog);
    out << infolog << std::endl;
    free(infolog);
  }
}

void print_shader_source(GLhandleARB shader, std::ostream& out)
{
  GLint source_len;
  glGetObjectParameterivARB(shader, GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &source_len);
  
  if (source_len > 0) {
    char* source = (char*)malloc(source_len);
    GLint nb_chars;
    glGetShaderSourceARB(shader, source_len, &nb_chars, source);

    std::stringstream ss(source);
    for (int i=1; !ss.eof(); i++) {
      std::string line;
      std::getline(ss, line);
      out.width(4); out << i;
      out.width(0); out << ":  " << line << std::endl;
    }

    free(source);
  }
}

string glsl_typename(ShValueType type, int size)
{
  stringstream s;
  if (shIsInteger(type)) {
    if (size > 1) {
      s << "ivec";
    } else {
      return "int";
    }
  } else {
    if (size > 1) {
      s << "vec";
    } else {
      return "float";
    }
  }
  s << size;

  return s.str();
}

}
