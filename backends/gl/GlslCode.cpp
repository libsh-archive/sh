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
    m_nb_variables(0), m_nb_varying(0), m_gl_Normal_allocated(false),
    m_gl_Position_allocated(false), m_gl_FragColor_allocated(false),
    m_uploaded(false)
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
}

void GlslCode::generate()
{
  ShProgramNodePtr temp_shader = m_originalShader->clone();
  m_shader = temp_shader.object();
  m_shader->acquireRef();
  temp_shader = NULL;

  ShContext::current()->enter(m_shader);
  
  ShTransformer transform(m_shader);
  //transform.convertInputOutput(); 
  //transform.convertTextureLookups();
  //transform.convertToFloat(m_convertMap);
  //transform.splitTuples(4, m_splits);
  //transform.stripDummyOps();

  if (transform.changed()) {
    optimize(m_shader);
  } else {
    m_shader->releaseRef();
    m_shader = m_originalShader;
    ShContext::current()->exit();
    ShContext::current()->enter(m_shader);
  }
  
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
  string s = code.str(); // w/o this extra copy, the shader code gets corrupted

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
  //glDeleteObjectARB(m_arb_shader); // Mark for deletion on detachment

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

string GlslCode::type_string(const GlslVariable &v)
{
  stringstream s;
  if (shIsInteger(v.type)) {
    if (v.size > 1) {
      s << "ivec";
    } else {
      return "int";
    }
  } else {
    if (v.size > 1) {
      s << "vec";
    } else {
      return "float";
    }
  }

  s << v.size;
  return s.str();
}

string GlslCode::declare_var(const GlslVariable& v)
{
  if (v.builtin) return "";

  string ret;
  string type = type_string(v);

  if (v.kind == SH_CONST) {
    ret += "const ";
  }
  ret += type + " " + v.name;
  if (!v.values.empty()) {
    ret += " = " + type + "(" + v.values + ")";
  }

  return ret;
}

ostream& GlslCode::print(ostream& out)
{
  out << "// OpenGL SL " << ((m_unit == SH_GLSL_VP) ? "Vertex" : "Fragment") << " Program" << endl;
  out << endl;

  vector<string> vars; // holds variables to be declared in main
  for (map<ShVariableNodePtr, GlslVariable>::const_iterator i = m_varmap.begin(); 
       i != m_varmap.end(); i++) {
    string var = declare_var((*i).second);

    if (var.empty()) continue; // builtin variables are implicitly declared

    if ((*i).second.varying()) {
      out << "varying " << var << ";" << endl;
    } else {
      vars.push_back(var);
    }
  }
  out << endl;

  out << "void main()" << endl;
  out << "{" << endl;
  string indent = "  ";

  // Temporaries and constants
  for (vector<string>::const_iterator i = vars.begin(); i != vars.end(); i++) {
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

string GlslCode::var_name(const ShVariable& v)
{
  stringstream varname;
  switch (v.node()->kind()) {
  case SH_INPUT:
    varname << "var_i_";
    break;
  case SH_OUTPUT:
    varname << "var_o_";
    break;
  case SH_INOUT:
    varname << "var_io_";
    break;
  case SH_TEMP:
    varname << "var_t_";
    break;
  case SH_CONST:
    varname << "var_c_";
    break;
  case SH_TEXTURE:
    varname << "var_tex_";
    break;
  case SH_STREAM:
    varname << "var_s_";
    break;
  case SH_PALETTE:
    varname << "var_p_";
    break;
  default:
    varname << "var_";
  }
  varname << m_nb_variables++;
  return varname.str();
}

void GlslCode::allocate_var(const ShVariable& v)
{
  GlslVariable var;
  var.builtin = false;
  var.size = v.node()->size();
  var.kind = v.node()->kind();
  var.type = v.valueType();
  var.semantic_type = v.node()->specialType();
  if (v.hasValues()) {
    var.values = v.getVariant()->encodeArray();
  }
  
  // Special cases
  if (!m_gl_Position_allocated && (var.semantic_type == SH_POSITION) &&
      ((var.kind == SH_INPUT) || (var.kind == SH_INOUT) || (var.kind == SH_OUTPUT)) ) {
    var.name = "gl_Position";
    var.builtin = true;
    m_gl_Position_allocated = true;
  }
  else if (!m_gl_FragColor_allocated && (var.semantic_type == SH_COLOR) &&
	   ((var.kind == SH_OUTPUT) || (var.kind == SH_INOUT))) {
    var.name = "gl_FragColor";
    var.builtin = true;
    m_gl_FragColor_allocated = true;
  }
  else if (!m_gl_Normal_allocated && (m_unit == SH_GLSL_VP) && (var.semantic_type == SH_NORMAL) &&
	   ((var.kind == SH_INPUT) || (var.kind == SH_INOUT) || (var.kind == SH_OUTPUT)) ) {
    stringstream ss;
    ss << "varying" << m_nb_varying++;
    var.name = ss.str();

    var.values = "gl_Normal";
    m_gl_Normal_allocated = true;
  }
  else if (var.varying()) {
    stringstream ss;
    ss << "varying" << m_nb_varying++;
    var.name = ss.str();
  }
  else {
    var.name = var_name(v);  
  }

  m_varmap[v.node()] = var;
}

string GlslCode::resolve(const ShVariable& v)
{
  if (m_varmap.find(v.node()) == m_varmap.end()) {
    allocate_var(v);
  }
  return m_varmap[v.node()].name + swizzle(v);
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
