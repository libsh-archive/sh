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
#include <iostream>

#include "ShStorageType.hpp"

#define SH_DEBUG_GLSL_BACKEND

namespace shgl {

GLhandleARB GlslCode::m_arb_program = 0;
GlslCode* GlslCode::m_current_shaders[] = { NULL, NULL };

using namespace SH;
using namespace std;

GlslCode::GlslCode(const ShProgramNodeCPtr& shader, const std::string& unit,
		   TextureStrategy* texture) 
  : m_texture(texture), m_target("glsl:" + unit), m_arb_shader(0),
    m_varmap(NULL), m_nb_textures(0), m_bound(false)
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
    unbind();
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
  GLint code_len = strlen(code_string);
  SH_GL_CHECK_ERROR(glShaderSourceARB(m_arb_shader, 1, &code_string, &code_len));
  SH_GL_CHECK_ERROR(glCompileShaderARB(m_arb_shader));

#ifdef SH_DEBUG_GLSL_BACKEND
  // Check compilation
  int compiled;
  glGetObjectParameterivARB(m_arb_shader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
  if (compiled != GL_TRUE) {
    cout << "Shader compile status (target = " << m_target << "): FAILED" << endl << endl;
    cout << "Shader infolog:" << endl;
    print_infolog(m_arb_shader);
    cout << "Shader code:" << endl;
    print_shader_source(m_arb_shader);
    cout << endl;
    return;
  }
#endif
}

void GlslCode::link()
{
  SH_DEBUG_ASSERT(m_arb_program);

  SH_GL_CHECK_ERROR(glLinkProgramARB(m_arb_program));
  
#ifdef SH_DEBUG_GLSL_BACKEND
  // Check linking
  int linked;
  glGetObjectParameterivARB(m_arb_program, GL_OBJECT_LINK_STATUS_ARB, &linked);
  if (linked != GL_TRUE) {
    cout << "Program link status (target = " << m_target << "): FAILED" << endl << endl;
    cout << "Program infolog:" << endl;
    print_infolog(m_arb_program);
    cout << "Program code:" << endl;
    print_shader_source(m_arb_program);
    cout << endl;
    return;
  }
#endif

  SH_GL_CHECK_ERROR(glUseProgramObjectARB(m_arb_program));

#ifdef SH_DEBUG_GLSL_BACKEND
  // This could be slow, it should not be enabled in release code
  glValidateProgramARB(m_arb_program);
  int validated;
  glGetObjectParameterivARB(m_arb_program, GL_OBJECT_VALIDATE_STATUS_ARB, &validated);
  if (validated != GL_TRUE) {
    cout << "Program validate status (target = " << m_target << "): FAILED" << endl;
    cout << "Program infolog:" << endl;
    print_infolog(m_arb_program);
  }
#endif

  if (m_bound) {
    SH_DEBUG_ASSERT(m_varmap);

    // Whenever the program is linked, we must reinitialize the uniforms
    // because their values are reset to 0.
    //
    // NOTE: we must callglUseProgramObjectARB before we can get the
    // location of uniforms.
    for (GlslVariableMap::NodeList::iterator i = m_varmap->node_begin();
	 i != m_varmap->node_end(); i++) {
      ShVariableNodePtr node = *i;
      if (node->hasValues() && node->uniform()) {
	updateUniform(node);
      }
    }
  }
}

void GlslCode::bind()
{
  if (m_bound) {
    update();
    return;
  }

  // Unbind the previously attached shader if necessary
  if (m_current_shaders[m_unit]) {
    m_current_shaders[m_unit]->unbind();
  }
  
  // Compile code if necessary
  if (!m_arb_shader) {
    upload();
  }

  if (!m_arb_program) {
    m_arb_program = glCreateProgramObjectARB();
  }
  
  ShContext::current()->set_binding(m_target, ShProgram(m_originalShader));
  m_current_shaders[m_unit] = this;
  m_bound = true; // must be set before the call to link()
  
  SH_GL_CHECK_ERROR(glAttachObjectARB(m_arb_program, m_arb_shader));
  link();

  // Make sure all textures are loaded.
  bind_textures();
}

void GlslCode::unbind()
{
  if (!m_bound) return;

  SH_DEBUG_ASSERT(m_current_shaders[m_unit] == this);
  SH_DEBUG_ASSERT(m_arb_shader);
  SH_DEBUG_ASSERT(m_arb_program);

  SH_GL_CHECK_ERROR(glDetachObjectARB(m_arb_program, m_arb_shader));

  m_bound = false; // must be set before the calls to link() and unset_binding()
  m_current_shaders[m_unit] = NULL;

  // Refresh the current rendering state
  if (!m_current_shaders[SH_GLSL_FP] && !m_current_shaders[SH_GLSL_VP]) {
    SH_GL_CHECK_ERROR(glUseProgramObjectARB(0));
  } else {
    link();
  }
  
  ShContext::current()->unset_binding(m_target); // calls the destructor
}

void GlslCode::update()
{
  if (!m_bound) return;
  bind_textures();
}

void GlslCode::updateFloatUniform(const ShVariableNodePtr& node, const GLint location)
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

void GlslCode::updateIntUniform(const ShVariableNodePtr& node, const GLint location)
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

  SH_DEBUG_ASSERT(m_arb_program);
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
  
  if (var.texture()) return;

  GLint location = glGetUniformLocationARB(m_arb_program, var.name().c_str());
  if (location != -1) {
    if (shIsInteger(uniform->valueType())) {
      updateIntUniform(uniform, location);
    } else {
      updateFloatUniform(uniform, location);
    }
  } else {
    cerr << "Cannot find uniform named '" << var.name() << "'." << endl;
  }
}

ostream& GlslCode::print(ostream& out)
{
  SH_DEBUG_ASSERT(m_varmap);

  out << "// OpenGL SL " << ((m_unit == SH_GLSL_VP) ? "Vertex" : "Fragment") << " Program" << endl;
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
  string indent = "  ";

  // Declare and initialize temporaries and constants
  if (m_varmap->regular_begin() != m_varmap->regular_end()) {
    for (GlslVariableMap::DeclarationList::const_iterator i = m_varmap->regular_begin(); 
	 i != m_varmap->regular_end(); i++) {
      out << indent << *i << ";" << endl;
    }
    out << endl;
  }

  // Print lines of code
  for (vector<string>::const_iterator i = m_lines.begin(); i != m_lines.end(); i++) {
    out << indent << *i << ";" << endl;
  }
  
  out << "}" << endl;
  return out;
}

ostream& GlslCode::describe_interface(ostream& out)
{
  // TODO
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

string GlslCode::resolve(const ShVariable& v, int index) const
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
  SH_DEBUG_ASSERT(m_arb_program);

  for (ShProgramNode::TexList::const_iterator i = m_shader->textures.begin();
       i != m_shader->textures.end(); i++) {
    ShTextureNodePtr texture = *i;

    if (m_texture_units.find(texture) == m_texture_units.end()) {
      cerr << "Texture '" << texture->name() << "' has no assigned unit." << endl;
      continue;
    }
    
    const GlslVariable& var(m_varmap->variable(shref_dynamic_cast<ShVariableNode>(texture)));
    GLint location = glGetUniformLocationARB(m_arb_program, var.name().c_str());

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

}
