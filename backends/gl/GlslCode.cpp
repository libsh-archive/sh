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
#include <vector>
#include <string>

namespace shgl {

using namespace SH;
using namespace std;

GlslCode::GlslCode(const ShProgramNodeCPtr& shader, const std::string& unit,
		   TextureStrategy* texture) : m_texture(texture), m_nb_variables(0)
{
  m_originalShader = const_cast<ShProgramNode*>(shader.object());
  
  if (unit == "fragment") m_unit = SH_GLSL_FP;
  if (unit == "vertex") m_unit = SH_GLSL_VP;
}

GlslCode::~GlslCode()
{
  if (m_shader != m_originalShader) {
    delete m_shader;
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
    genNode(m_shader->ctrlGraph->entry());
    m_shader->ctrlGraph->entry()->clearMarked();
  } 
  catch (...) {
    m_shader->ctrlGraph->entry()->clearMarked();
    ShContext::current()->exit();
    throw;
  }

  ShContext::current()->exit();
}

void GlslCode::upload()
{
}

void GlslCode::bind()
{
}

void GlslCode::update()
{
}

void GlslCode::updateUniform(const ShVariableNodePtr& uniform)
{
}

string GlslCode::type_string(const GlslVariable &var)
{
  stringstream s;
  if (shIsInteger(var.type)) {
    if (var.size > 1) {
      s << "ivec";
    } else {
      return "int";
    }
  } else {
    if (var.size > 1) {
      s << "vec";
    } else {
      return "float";
    }
  }

  s << var.size;
  return s.str();
}

ostream& GlslCode::print(ostream& out)
{
  out << "// OpenGL SL " << ((m_unit == SH_GLSL_VP) ? "Vertex" : "Fragment") << " Program" << endl;
  out << endl;

  out << "void main()" << endl;
  out << "{" << endl;
  string indent = "  ";

  // Declare variables
  for (map<ShVariableNodePtr, GlslVariable>::const_iterator i = m_varmap.begin(); 
       i != m_varmap.end(); i++) {
    out << indent << type_string((*i).second) << " " << (*i).second.name << ";" << endl;
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

void GlslCode::genNode(ShCtrlGraphNodePtr node)
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
  
  genNode(node->follower);
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

void GlslCode::allocate_var(const ShVariable& v)
{
  GlslVariable var;

  stringstream varname;
  varname << "v" << m_nb_variables++;
  var.name = varname.str();
  var.size = v.node()->size();
  var.type = v.valueType();

  m_varmap[v.node()] = var;
}

string GlslCode::resolve(const ShVariable& v)
{
  if (m_varmap.find(v.node()) == m_varmap.end()) {
    allocate_var(v);
  }
  return m_varmap[v.node()].name;
}

string GlslCode::resolve(const ShVariable& v, int idx)
{
  stringstream ss;
  ss << resolve(v) << "[" << idx << "]";
  return ss.str();
}

}
