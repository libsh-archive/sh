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
#include "GlslCode.hpp"
#include "ShTransformer.hpp"
#include "ShStorageType.hpp"
#include <iostream>

#define SH_DEBUG_GLSL_BACKEND // DEBUG

namespace shgl {

GLhandleARB GlslCode::m_arb_program = glCreateProgramObjectARB();

using namespace SH;
using namespace std;

GlslCode::GlslCode(const ShProgramNodeCPtr& shader, const std::string& unit,
		   TextureStrategy* texture) 
  : m_texture(texture), m_target("glsl:" + unit), m_arb_shader(0),
    m_varmap(NULL), m_uploaded(false)
{
  m_originalShader = const_cast<ShProgramNode*>(shader.object());
  
  if (unit == "fragment"){
    m_arb_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    m_unit = SH_GLSL_FP;
  }
  
  if (unit == "vertex") {
    m_arb_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    m_unit = SH_GLSL_VP;
  }
}

GlslCode::~GlslCode()
{
  if (m_shader != m_originalShader) {
    delete m_shader;
  }

  if (m_arb_shader != 0) {
    glDetachObjectARB(m_arb_program, m_arb_shader);
  }

  delete m_varmap;
}

void GlslCode::generate()
{
  ShProgramNodePtr temp_shader = m_originalShader->clone();
  m_shader = temp_shader.object();
  m_shader->acquireRef();
  temp_shader = NULL;

  ShContext::current()->enter(m_shader);
  
  ShTransformer transform(m_shader);
  transform.convertInputOutput(); 
  transform.convertTextureLookups();
  //transform.convertToFloat(m_convertMap);
  //transform.splitTuples(4, m_splits);
  transform.stripDummyOps();

  if (transform.changed()) {
    optimize(m_shader);
  } else {
    m_shader->releaseRef();
    m_shader = m_originalShader;
    ShContext::current()->exit();
    ShContext::current()->enter(m_shader);
  }

  // Initialize the variable map
  delete m_varmap;
  m_varmap = new GlslVariableMap(m_shader, m_unit);
  
  // Code generation
  try {
    m_shader->ctrlGraph->entry()->clearMarked();
    gen_node(m_shader->ctrlGraph->entry());
    m_shader->ctrlGraph->entry()->clearMarked();
  } 
  catch (...) {
    m_shader->ctrlGraph->entry()->clearMarked();
    ShContext::current()->exit();
    throw;
  }

  ShContext::current()->exit();
}

void GlslCode::print_infolog(GLhandleARB obj)
{
  int infolog_len;
  glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infolog_len);
  
  if (infolog_len > 0) {
    char* infolog = (char*)malloc(infolog_len);
    int nb_chars;
    glGetInfoLogARB(obj, infolog_len, &nb_chars, infolog);
    cout << infolog << endl;
    free(infolog);
  }
}

void GlslCode::print_shader_source()
{
  int source_len;
  glGetObjectParameterivARB(m_arb_shader, GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &source_len);
  
  if (source_len > 0) {
    char* source = (char*)malloc(source_len);
    int nb_chars;
    glGetShaderSourceARB(m_arb_shader, source_len, &nb_chars, source);

    stringstream ss(source);
    for (int i=1; !ss.eof(); i++) {
      char line[1024];
      ss.getline(line, sizeof(line));
      cout.width(4); cout << i;
      cout.width(0); cout << ":  " << line << endl;
    }

    free(source);
  }
}

void GlslCode::upload()
{
  stringstream code;
  print(code);
  string s = code.str(); // w/o this extra copy, the last bytes of the shader code become garbage

  const char* code_string = s.c_str();
  GLint code_len = strlen(code_string);
  glShaderSourceARB(m_arb_shader, 1, &code_string, &code_len);
  glCompileShaderARB(m_arb_shader);

  // Check compilation
  int compiled;
  glGetObjectParameterivARB(m_arb_shader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
  if (compiled != GL_TRUE) {
    cout << "Shader compile status (target = " << m_target << "): FAILED" << endl << endl;
    cout << "Shader infolog:" << endl;
    print_infolog(m_arb_shader);
    cout << "Shader code:" << endl;
    print_shader_source();
    cout << endl;
    return;
  }

  glAttachObjectARB(m_arb_program, m_arb_shader);
  glDeleteObjectARB(m_arb_shader); // mark for deletion on detachment

#ifdef SH_DEBUG_GLSL_BACKEND
  int nb_objects;
  glGetObjectParameterivARB(m_arb_program, GL_OBJECT_ATTACHED_OBJECTS_ARB, &nb_objects);
  cout << "Number of objects attached to the program (target = " << m_target << "): " << nb_objects << endl;
#endif

  m_uploaded = true;
}

void GlslCode::bind()
{
  if (!m_uploaded) {
    upload();
  }

  glLinkProgramARB(m_arb_program);

  // Check linking
  int linked;
  glGetObjectParameterivARB(m_arb_program, GL_OBJECT_LINK_STATUS_ARB, &linked);
  if (linked != GL_TRUE) {
    cout << "Program link status (target = " << m_target << "): FAILED" << endl << endl;
    cout << "Program infolog:" << endl;
    print_infolog(m_arb_program);
    cout << "Shader code:" << endl;
    print_shader_source();
    cout << endl;
    return;
  }

  glUseProgramObjectARB(m_arb_program);
  ShContext::current()->set_binding(m_target, ShProgram(m_originalShader));

#ifdef SH_DEBUG_GLSL_BACKEND
  cout << "Program validate status (target = " << m_target << "): ";
  glValidateProgramARB(m_arb_program);
  int validated;
  glGetObjectParameterivARB(m_arb_program, GL_OBJECT_VALIDATE_STATUS_ARB, &validated);
  cout << (validated == GL_TRUE ? "OK" : "FAILED") << endl;
  if (validated != GL_TRUE) {
    cout << "Program infolog:" << endl;
    print_infolog(m_arb_program);
  }
#endif
}

void GlslCode::update()
{
}

void GlslCode::updateUniform(const ShVariableNodePtr& uniform)
{
  // After updating the uniform variables, we must relink the shaders (hence, call bind())
}

ostream& GlslCode::print(ostream& out)
{
  SH_DEBUG_ASSERT(m_varmap);

  out << "// OpenGL SL " << ((m_unit == SH_GLSL_VP) ? "Vertex" : "Fragment") << " Program" << endl;
  out << endl;

  // Varying variables
  for (GlslVariableMap::DeclarationList::const_iterator i = m_varmap->varying_begin(); 
       i != m_varmap->varying_end(); i++) {
    out  << "varying " << *i << ";" << endl;
  }
  out << endl;

  out << "void main()" << endl;
  out << "{" << endl;
  string indent = "  ";

  // Temporaries and constants
  for (GlslVariableMap::DeclarationList::const_iterator i = m_varmap->regular_begin(); 
       i != m_varmap->regular_end(); i++) {
    out << indent << *i << ";" << endl;
  }
  out << endl;

  // Print lines of code
  for (vector<string>::const_iterator i = m_lines.begin(); i != m_lines.end(); i++) {
    out << indent << *i << ";" << endl;
  }
  
  out << "}" << endl;
  return out;
}

ostream& GlslCode::describe_interface(ostream& out)
{
  return out;
}

void GlslCode::optimize(const ShProgramNodeCPtr& shader)
{
}

void GlslCode::gen_node(ShCtrlGraphNodePtr node)
{
  if (!node || node->marked()) return;
  node->mark();

  if (node == m_shader->ctrlGraph->exit()) return;
  
  if (node->block) {
    for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
	 I != node->block->end(); ++I) {
      const ShStatement& stmt = *I;
      emit(stmt);
    }
  }
  
  gen_node(node->follower);
}

void GlslCode::emit(const ShStatement &stmt)
{
  switch(stmt.op) {
  case SH_OP_ADD:
    {
      m_lines.push_back(resolve(stmt.dest) + " = " + resolve(stmt.src[0]) + " + " + resolve(stmt.src[1]));
      break;
    }
  case SH_OP_MUL:
    {
      m_lines.push_back(resolve(stmt.dest) + " = " + resolve(stmt.src[0]) + " * " + resolve(stmt.src[1]));
      break;
    }
  case SH_OP_DOT:
    {
      SH_DEBUG_ASSERT(stmt.dest.size() == 1);
      m_lines.push_back(resolve(stmt.dest) + " = dot(" + resolve(stmt.src[0]) + ", " + resolve(stmt.src[1]) + ")");
      break;
    }
  case SH_OP_ASN:
    {
      m_lines.push_back(resolve(stmt.dest) + " = " + resolve(stmt.src[0]));
      break;
    }
  case SH_OP_NORM:
    {
      m_lines.push_back(resolve(stmt.dest) + " = normalize(" + resolve(stmt.src[0]) + ")");
      break;
    }
  default:
    m_lines.push_back("// *** unhandled operation " + 
		      string(opInfo[stmt.op].name) + " ***");
    break;
  }
}

string GlslCode::resolve(const ShVariable& v)
{
  SH_DEBUG_ASSERT(m_varmap);
  std::string s = m_varmap->resolve(v.node()).name() + swizzle(v);
  if (v.neg()) {
    s = std::string("-(") + s + ")";
  }
  return s;
}

string GlslCode::resolve(const ShVariable& v, int idx)
{
  stringstream ss;
  ss << resolve(v) << "[" << idx << "]";
  return ss.str() + swizzle(v);
}

string GlslCode::swizzle(const ShVariable& v)
{
  const ShSwizzle& s(v.swizzle());
  if (s.identity()) return "";

  stringstream ss;
  for (int i=0; i < v.size(); i++) {
    switch (s[i]) {
    case 0:
      ss << "x";
      break;
    case 1:
      ss << "y";
      break;
    case 2:
      ss << "z";
      break;
    case 3:
      ss << "w";
      break;
    default:
      ss << " *** ERROR: Invalid swizzle '" << s[i] << "' *** ";
      return ss.str();
    }
  }
  return "." + ss.str();
}

}
