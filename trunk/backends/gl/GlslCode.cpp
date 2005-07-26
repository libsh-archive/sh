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
#include "GlslCode.hpp"
#include <iostream>

#include "ShStorageType.hpp"
#include "ShOptimizations.hpp"

//#define SH_GLSL_DEBUG

namespace shgl {

using namespace SH;
using namespace std;

struct GlslError : public ShException {
public:
  GlslError(const std::string& message)
    : ShException(std::string("GLSL Backend: ") + message)
  {
  }
};

GlslSet* GlslSet::m_current = 0;

GlslCode::GlslCode(const ShProgramNodeCPtr& shader, const std::string& unit,
		   TextureStrategy* texture) 
  : m_texture(texture), m_target("glsl:" + unit), m_arb_shader(0),
    m_indent(0), m_varmap(NULL), m_nb_textures(0), m_bound(0)
{
  m_originalShader = const_cast<ShProgramNode*>(shader.object());
  
  if (unit == "fragment"){
    m_unit = SH_GLSL_FP;
  } else if (unit == "vertex") {
    m_unit = SH_GLSL_VP;
  }
}

GlslCode::~GlslCode()
{
  if (m_shader != m_originalShader) {
    delete m_shader;
  }

  if (m_arb_shader != 0) {
    // Shouldn't be bound, ever
    SH_DEBUG_ASSERT(!m_bound);
    SH_GL_CHECK_ERROR(glDeleteObjectARB(m_arb_shader));
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

  SH::ShTransformer::ValueTypeMap convert_map;
  convert_map[SH_DOUBLE] = SH_FLOAT; 
  convert_map[SH_HALF] = SH_FLOAT;
  convert_map[SH_INT] = SH_FLOAT;
  convert_map[SH_SHORT] = SH_FLOAT;
  convert_map[SH_BYTE] = SH_FLOAT;
  convert_map[SH_UINT] = SH_FLOAT;
  convert_map[SH_USHORT] = SH_FLOAT;
  convert_map[SH_UBYTE] = SH_FLOAT;
  convert_map[SH_FINT] = SH_FLOAT;
  convert_map[SH_FSHORT] = SH_FLOAT;
  convert_map[SH_FBYTE] = SH_FLOAT;
  convert_map[SH_FUINT] = SH_FLOAT;
  convert_map[SH_FUSHORT] = SH_FLOAT;
  convert_map[SH_FUBYTE] = SH_FLOAT;
  
  transform.convertToFloat(convert_map);
  transform.splitTuples(4, m_splits);
  transform.stripDummyOps();
  transform.expand_atan2();
  transform.expand_inverse_hyperbolic();

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
    ShStructural structural(m_shader->ctrlGraph);
    m_shader->ctrlGraph->entry()->clearMarked();
    gen_structural_node(structural.head());
    m_shader->ctrlGraph->entry()->clearMarked();
    allocate_textures();
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
  if (SH_GLSL_VP == m_unit) {
    m_arb_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  } else if (SH_GLSL_FP == m_unit) {
    m_arb_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
  }
  SH_DEBUG_ASSERT(m_arb_shader);

  stringstream code;
  print(code);
  string s = code.str(); // w/o this extra copy, the last bytes of the shader code become garbage

  const char* code_string = s.c_str();
  GLint code_len = s.size();
  SH_GL_CHECK_ERROR(glShaderSourceARB(m_arb_shader, 1, &code_string, &code_len));
  SH_GL_CHECK_ERROR(glCompileShaderARB(m_arb_shader));

  // Check compilation
  GLint compiled;
  glGetObjectParameterivARB(m_arb_shader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
  if (compiled != GL_TRUE) {
    std::ostringstream os;
    os << "Shader compile status (target = " << m_target << "): FAILED" << endl << endl;
    os << "Shader infolog:" << endl;
    print_infolog(m_arb_shader, os);
    os << "Shader code:" << endl;
    print_shader_source(m_arb_shader, os);
    os << endl;
    shError(GlslError(os.str()));
    return;
#ifdef SH_GLSL_DEBUG
  } else {
    std::ostringstream os;
    os << "Shader compile status (target = " << m_target << "): OK" << endl << endl;
    os << "Shader infolog:" << endl;
    print_infolog(m_arb_shader, os);
    os << "Shader code:" << endl;
    print_shader_source(m_arb_shader, os);
    os << endl;
    cerr << os.str();
#endif
  }
}

GLhandleARB GlslCode::glsl_shader()
{
  if (!m_arb_shader) {
    upload();
  }

  return m_arb_shader;
}

void GlslCode::bind()
{
  if (m_bound) return;

  if (!m_fallback_set) m_fallback_set = new GlslSet;

  if (GlslSet::current()) {
    *m_fallback_set = *GlslSet::current();
    m_fallback_set->attach(this);
  } else {
    m_fallback_set->replace(this);
  }
  m_fallback_set->bind();
}

void GlslCode::unbind()
{
  if (!m_bound) return;
  SH_DEBUG_ASSERT(GlslSet::current());
  
  if (!m_fallback_set) m_fallback_set = new GlslSet;

  GlslSet* current = GlslSet::current();
  
  *m_fallback_set = *current;
  
  m_fallback_set->detach(this);

  if (m_fallback_set->empty()) {
    current->unbind();
  } else {
    m_fallback_set->bind();
  }
}

void GlslCode::update()
{
  if (!m_bound) return;
  bind_textures();
}

void GlslCode::set_bound(GLhandleARB program)
{
  m_bound = program;
  if (program) {
    ShContext::current()->set_binding(m_target, ShProgram(m_originalShader));
  } else {
    ShContext::current()->unset_binding(m_target);
  }
}

void GlslCode::upload_uniforms()
{
  if (!m_bound) return;
  
  SH_DEBUG_ASSERT(m_varmap);

  for (GlslVariableMap::NodeList::iterator i = m_varmap->node_begin();
       i != m_varmap->node_end(); i++) {
    ShVariableNodePtr node = *i;
    
    if (node->hasValues() && node->uniform()) {
      // Normal uniforms
      updateUniform(node); 
    } else {
      // Palettes are implemented as uniform arrays
      ShPaletteNodePtr palette = shref_dynamic_cast<ShPaletteNode>(*i);
      if (palette) {
	const GlslVariable& var(m_varmap->variable(palette));

	for (unsigned i=0; i < palette->palette_length(); i++) {
	  ShVariableNodePtr uniform = palette->get_node(i);
	  stringstream name;
	  name << var.name() << "[" << i << "]";
	  real_update_uniform(uniform, name.str().c_str());
	}
      }
    }
  }
}

void GlslCode::update_float_uniform(const ShVariableNodePtr& node, const GLint location)
{
  float values[4];
  for (int i=0; i < node->size(); i++) {
    values[i] = ((float*)(node->getVariant()->array()))[i];
  }

  switch (node->size()) {
  case 1:
    SH_GL_CHECK_ERROR(glUniform1fARB(location, values[0]));
    break;
  case 2:
    SH_GL_CHECK_ERROR(glUniform2fARB(location, values[0], values[1]));
    break;
  case 3:
    SH_GL_CHECK_ERROR(glUniform3fARB(location, values[0], values[1], values[2]));
    break;
  case 4:
    SH_GL_CHECK_ERROR(glUniform4fARB(location, values[0], values[1], values[2], values[3]));
    break;
  default:
    SH_DEBUG_ASSERT(0); // Unsupported size
  }
}

void GlslCode::update_int_uniform(const ShVariableNodePtr& node, const GLint location)
{
  int values[4];
  for (int i=0; i < node->size(); i++) {
    values[i] = ((int*)(node->getVariant()->array()))[i];
  }

  switch (node->size()) {
  case 1:
    SH_GL_CHECK_ERROR(glUniform1iARB(location, values[0]));
    break;
  case 2:
    SH_GL_CHECK_ERROR(glUniform2iARB(location, values[0], values[1]));
    break;
  case 3:
    SH_GL_CHECK_ERROR(glUniform3iARB(location, values[0], values[1], values[2]));
    break;
  case 4:
    SH_GL_CHECK_ERROR(glUniform4iARB(location, values[0], values[1], values[2], values[3]));
    break;
  default:
    SH_DEBUG_ASSERT(0); // Unsupported size
  }
}

void GlslCode::updateUniform(const ShVariableNodePtr& uniform)
{
  if (!m_bound) return;

  SH_DEBUG_ASSERT(uniform);

  if (!m_varmap->contains(uniform)) {
    // Check to see if the uniform was split
    if (m_splits.count(uniform) > 0) {
      ShTransformer::VarNodeVec &splitVec = m_splits[uniform];
      ShVariantCPtr uniformVariant = uniform->getVariant();

      int offset = 0;
      int copySwiz[4];
      for (ShTransformer::VarNodeVec::iterator it = splitVec.begin();
	   it != splitVec.end(); offset += (*it)->size(), ++it) {
	// TODO switch to properly swizzled version
        for (int i=0; i < (*it)->size(); ++i) {
	  copySwiz[i] = i + offset;
	}
        (*it)->setVariant(uniformVariant->get(false,
            ShSwizzle(uniform->size(), (*it)->size(), copySwiz))); 
        updateUniform(*it);
      }
    }
    return;
  }

  const GlslVariable& var(m_varmap->variable(uniform));
  
  if (!var.texture()) {
    real_update_uniform(uniform, var.name());
  }
}

void GlslCode::real_update_uniform(const ShVariableNodePtr& uniform, const string& name)
{
  // TODO: cache these?
  GLint location = glGetUniformLocationARB(m_bound, name.c_str());
  if (location != -1) {
    if (shIsInteger(uniform->valueType())) {
      update_int_uniform(uniform, location);
    } else {
      update_float_uniform(uniform, location);
    }
  }
}

ostream& GlslCode::print(ostream& out)
{
  SH_DEBUG_ASSERT(m_varmap);
  const string BLOCK_INDENT("    ");

  out << "// OpenGL " << ((m_unit == SH_GLSL_VP) ? "Vertex" : "Fragment") << " Program" << endl;
  out << endl;

  // Declare Uniforms
  if (m_varmap->uniform_begin() != m_varmap->uniform_end()) {
    for (GlslVariableMap::DeclarationList::const_iterator i = m_varmap->uniform_begin(); 
	 i != m_varmap->uniform_end(); i++) {
      out << "uniform " << *i << ";" << endl;
    }
    out << endl;
  }

  out << "void main()" << endl;
  out << "{" << endl;

  // Declare and initialize temporaries and constants
  if (m_varmap->regular_begin() != m_varmap->regular_end()) {
    for (GlslVariableMap::DeclarationList::const_iterator i = m_varmap->regular_begin(); 
	 i != m_varmap->regular_end(); i++) {
      out << BLOCK_INDENT << *i << ";" << endl;
    }
    out << endl;
  }

  // Print lines of code
  for (vector<GlslLine>::const_iterator i = m_lines.begin(); i != m_lines.end(); i++) {
    stringstream indent;
    for (int j=0; j <= (*i).indent; j++) indent << BLOCK_INDENT;
    out << indent.str() << (*i).code << endl;
  }
  
  out << "}" << endl;
  return out;
}

ostream& GlslCode::describe_interface(ostream& out)
{
  // TODO
  return out;
}

void GlslCode::append_line(const string& line, bool append_semicolon)
{
  GlslLine l;
  l.indent = m_indent;
  l.code = append_semicolon ? line + ";" : line;  
  m_lines.push_back(l);
}

void GlslCode::gen_structural_node(const ShStructuralNodePtr& node)
{
  if (!node) return;

  if (node->type == ShStructuralNode::UNREDUCED) {
    ShBasicBlockPtr block = node->cfg_node->block;
    if (block) for (ShBasicBlock::ShStmtList::const_iterator I = block->begin();
                    I != block->end(); ++I) {
      const ShStatement& stmt = *I;
      emit(stmt);
    }
  } 
  else if (node->type == ShStructuralNode::BLOCK) {
    for (ShStructuralNode::StructNodeList::const_iterator I = node->structnodes.begin();
         I != node->structnodes.end(); ++I) {
      gen_structural_node(*I);
    }
  } 
  else if (node->type == ShStructuralNode::IFELSE) {
    ShStructuralNodePtr header = node->structnodes.front();
    SH_DEBUG_ASSERT(2 == header->succs.size());

    ShVariable cond;
    ShStructuralNodePtr ifnode, elsenode;
    for (ShStructuralNode::SuccessorList::iterator I = header->succs.begin();
         I != header->succs.end(); ++I) {
      if (I->first.node()) {
        ifnode = I->second;
        cond = I->first;
      } else {
        elsenode = I->second;
      }
    }
    gen_structural_node(header);
    append_line("if (bool(" + resolve(cond.node()) + ")) {", false);
    m_indent++;
    gen_structural_node(ifnode);
    m_indent--;

    append_line("} else {", false);
    m_indent++;
    gen_structural_node(elsenode);
    m_indent--;

    append_line("} // if", false);
  } 
  else if (node->type == ShStructuralNode::WHILELOOP) {
    ShStructuralNodePtr header = node->structnodes.front();
    ShVariable cond = header->succs.front().first;
    ShStructuralNodePtr body = node->structnodes.back();

    append_line("// evaluate loop condition:", false);
    gen_structural_node(header);
    append_line("while (bool(" + resolve(cond.node()) + ")) {", false);

    m_indent++;
    append_line("// execute loop body:", false);
    gen_structural_node(body);
    append_line("// evaluate loop condition:", false);
    gen_structural_node(header);
    m_indent--;
    
    append_line("} // while", false);
  } 
  else if (node->type == ShStructuralNode::SELFLOOP) {
    ShStructuralNodePtr loopnode = node->structnodes.front();

    bool condexit = true; // true if the condition causes us to exit the
                          // loop, rather than continue it
    ShVariable cond;
    for (ShStructuralNode::SuccessorList::iterator I = loopnode->succs.begin();
         I != loopnode->succs.end(); ++I) {
      if (I->first.node()) {
        condexit = (I->second != loopnode);
        cond = I->first;
      }
    }

    // We convert the loop to a while loop since do..while loops don't
    // compile properly on NVIDIA
    append_line("// execute do-until body at least once:", false);
    gen_structural_node(loopnode);

    append_line("while (" + string((condexit) ? "!" : "") + "(bool(" + resolve(cond.node()) + "))) {", false);

    m_indent++;
    gen_structural_node(loopnode);
    m_indent--;

    append_line("} // while", false);
  }
}

string GlslCode::resolve(const ShVariable& v, int index) const
{
  SH_DEBUG_ASSERT(m_varmap);
  return m_varmap->resolve(v, index);
}

string GlslCode::resolve(const ShVariable& v, const ShVariable& index) const
{
  SH_DEBUG_ASSERT(m_varmap);
  return m_varmap->resolve(v, index);
}

void GlslCode::allocate_textures()
{
  list<GLuint> reserved;

  // reserve any texunits specified at the program level
  if (!m_shader->meta("opengl:reservetex").empty()) {
    GLuint index;
    istringstream is(m_shader->meta("opengl:reservetex"));

    while(1) {
      is >> index;
      if (!is) break;
      reserved.push_back(index);
    }
  }

  // reserve and allocate any preset texunits
  for (ShProgramNode::TexList::const_iterator i = m_shader->textures.begin();
       i != m_shader->textures.end(); i++) {
    ShTextureNodePtr node = *i;
    
    if (!node->meta("opengl:texunit").empty() ||
	!node->meta("opengl:preset").empty()) {
      GLuint index;
      std::istringstream is;
      
      if (!node->meta("opengl:texunit").empty()) {
	is.str(node->meta("opengl:texunit"));
      } else {
	is.str(node->meta("opengl:preset"));
      }
      
      is >> index; // TODO: Check for errors
      
      if (find(reserved.begin(), reserved.end(), index) == reserved.end()) {
	m_texture_units[node].index = index;
	m_texture_units[node].preset = true;
	reserved.push_back(index);
      } else {
	cerr << "Texture unit " << index << " is used more than once." << endl;
      }
    }
  }
  
  // allocate remaining textures units with respect to the reserved list
  for (ShProgramNode::TexList::const_iterator i = m_shader->textures.begin();
       i != m_shader->textures.end(); i++) {
    ShTextureNodePtr node = *i;
    
    if (node->meta("opengl:texunit").empty() &&
	node->meta("opengl:preset").empty()) {
      GLuint index;
      const unsigned MAX_TEXTURE_UNITS = (SH_GLSL_VP == m_unit ?
					  GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB :
					  GL_MAX_TEXTURE_IMAGE_UNITS_ARB);
      
      for (index = m_nb_textures; index < MAX_TEXTURE_UNITS; index++) {
	if (find(reserved.begin(), reserved.end(), index) == reserved.end()) {
	  break;
	}
      }
      
      if (index < MAX_TEXTURE_UNITS) {
	m_texture_units[node].index = index;
      } else {
	cerr << "No available texture unit for '" << node->name() << "'." << endl;
      }

      m_nb_textures = index + 1;
    }
  }
}

void GlslCode::bind_textures()
{
  if (!m_bound) return;
  
  for (ShProgramNode::TexList::const_iterator i = m_shader->textures.begin();
       i != m_shader->textures.end(); i++) {
    ShTextureNodePtr texture = *i;

    if (m_texture_units.find(texture) == m_texture_units.end()) {
      cerr << "Texture '" << texture->name() << "' has no assigned unit." << endl;
      continue;
    }
    
    const GlslVariable& var(m_varmap->variable(shref_dynamic_cast<ShVariableNode>(texture)));
    GLint location = glGetUniformLocationARB(m_bound, var.name().c_str());

    if (location != -1) {
      int index = m_texture_units[texture].index;

      SH_GL_CHECK_ERROR(glUniform1iARB(location, index));
      
      if (!m_texture_units[texture].preset) {
	m_texture->bindTexture(texture, GL_TEXTURE0 + index);
      }
    } else {
      cerr << "Cannot find uniform texture named '" << var.name() << "'." << endl;
    }
  }
}

GlslSet* GlslCode::m_fallback_set = 0;

}
