// Sh: A GPU metaprogramming language.
//
// Copyright 2005 Serious Hack Inc.
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
#include "GlBackend.hpp"
#include "Glsl.hpp"
#include "GlslCode.hpp"
#include <iostream>

namespace shgl {

using namespace SH;

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

void print_infolog(GLhandleARB obj)
{
  int infolog_len;
  glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infolog_len);
  
  if (infolog_len > 0) {
    char* infolog = (char*)malloc(infolog_len);
    int nb_chars;
    glGetInfoLogARB(obj, infolog_len, &nb_chars, infolog);
    std::cout << infolog << std::endl;
    free(infolog);
  }
}

void print_shader_source(GLhandleARB shader)
{
  int source_len;
  glGetObjectParameterivARB(shader, GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &source_len);
  
  if (source_len > 0) {
    char* source = (char*)malloc(source_len);
    int nb_chars;
    glGetShaderSourceARB(shader, source_len, &nb_chars, source);

    std::stringstream ss(source);
    for (int i=1; !ss.eof(); i++) {
      char line[1024];
      ss.getline(line, sizeof(line));
      std::cout.width(4); std::cout << i;
      std::cout.width(0); std::cout << ":  " << line << std::endl;
    }

    free(source);
  }
}

}
