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
#include <fstream>

#include "StorageType.hpp"
#include "Optimizations.hpp"

//#define SH_GLSL_DEBUG

namespace shgl {

using namespace SH;
using namespace std;

struct GlslError : public Exception {
public:
  GlslError(const std::string& message)
    : Exception(std::string("GLSL Backend: ") + message)
  {
  }
};

GlslSet* GlslSet::m_current = 0;

GlslCode::GlslCode(const ProgramNodeCPtr& shader, const std::string& unit,
		   TextureStrategy* texture) 
  : m_texture(texture), m_target("glsl:" + unit), m_arb_shader(0),
    m_indent(0), m_varmap(NULL), m_nb_textures(0), m_bound(0)
{
  m_originalShader = const_cast<ProgramNode*>(shader.object());
  
  if (unit == "fragment"){
    m_unit = GLSL_FP;
  } else if (unit == "vertex") {
    m_unit = GLSL_VP;  
  }

  // Query the GL vendor string
  m_vendor = VENDOR_UNKNOWN;
  const GLubyte* vendor = SH_GL_CHECK_ERROR(glGetString(GL_VENDOR));
  if (vendor) {
    std::string s(reinterpret_cast<const char*>(vendor));
    if (s.find("NVIDIA") != s.npos) {
      m_vendor = VENDOR_NVIDIA;
    } else if( s.find("ATI") != s.npos) {
      m_vendor = VENDOR_ATI;
    } else {
      m_vendor = VENDOR_OTHER;
    }
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
  ProgramNodePtr temp_shader = m_originalShader->clone();
  m_shader = temp_shader.object();
  m_shader->acquireRef();
  temp_shader = NULL;

  Context::current()->enter(m_shader);

  Transformer transform(m_shader);
  VarTransformMap* original_vars = new VarTransformMap;
  transform.convertInputOutput(original_vars);
  transform.convertTextureLookups();

  Transformer::ValueTypeMap convert_map;
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
  //transform.texd_to_texlod();

  if (transform.changed()) {
    optimize(m_shader);
  } else {
    m_shader->releaseRef();
    m_shader = m_originalShader;
    Context::current()->exit();
    Context::current()->enter(m_shader);
  }

  // Initialize the extensions map
  m_glsl_extensions.clear();

  // Initialize the variable map
  delete m_varmap;
  m_varmap = new GlslVariableMap(m_shader, original_vars, m_unit);
  
  // Code generation
  try {
    Structural structural(m_shader->ctrlGraph);
    m_shader->ctrlGraph->entry()->clear_marked();
    gen_structural_node(structural.head());
#ifdef SH_GLSL_DEBUG
    std::ofstream f("structural.dot");
    structural.dump(f);
    std::system("dot -Tps structural.dot -o structural.ps");
#endif
    m_shader->ctrlGraph->entry()->clear_marked();
    allocate_textures();
  }
  catch (...) {
    m_shader->ctrlGraph->entry()->clear_marked();
    Context::current()->exit();
    throw;
  }

  Context::current()->exit();
}

void GlslCode::upload()
{
  if (GLSL_VP == m_unit) {
    m_arb_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  } else if (GLSL_FP == m_unit) {
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
    error(GlslError(os.str()));
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

// This must be done before linking the glsl program
void GlslCode::bind_generic_attributes(GLhandleARB glsl_program)
{
  SH_DEBUG_ASSERT(m_arb_shader);
  if (m_unit != GLSL_VP) return; // generic attributes are only for vertex programs right now

  for (ProgramNode::VarList::const_iterator i = m_shader->begin_inputs(); 
       i != m_shader->end_inputs(); i++) {
    const GlslVariable& var = m_varmap->variable(*i);
    if (var.attribute() >= 0) {
      SH_GL_CHECK_ERROR(glBindAttribLocationARB(glsl_program, var.attribute(), var.name().c_str()));
    }
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
    Context::current()->set_binding(m_target, Program(m_originalShader));
  } else {
    Context::current()->unset_binding(m_target);
  }
}

void GlslCode::upload_uniforms()
{
  if (!m_bound) return;
  
  SH_DEBUG_ASSERT(m_varmap);

  for (GlslVariableMap::NodeList::iterator i = m_varmap->node_begin();
       i != m_varmap->node_end(); i++) {
    VariableNodePtr node = *i;
    
    if (node->hasValues() && node->uniform()) {
      // Normal uniforms
      updateUniform(node); 
    } else {
      // Palettes are implemented as uniform arrays
      PaletteNodePtr palette = shref_dynamic_cast<PaletteNode>(*i);
      if (palette) {
        const GlslVariable& var(m_varmap->variable(palette));

        for (unsigned i=0; i < palette->palette_length(); i++) {
          VariableNodePtr uniform = palette->get_node(i);
          stringstream name;
          name << var.name() << "[" << i << "]";
          real_update_uniform(uniform, name.str().c_str());
        }
      }
    }
  }
}

void GlslCode::update_float_uniform(const VariableNodePtr& node, const GLint location)
{
  // Space for temporary data if we need it during data conversion
  GLfloat data[4];
  const GLfloat *values;

  const int uniform_size = node->size();
  if (node->valueType() == SH_FLOAT) {
    values = static_cast<const float *>(node->getVariant()->array());
  }
  else {
    // Componentwise cast to float and copy
    typedef DataVariant<GLfloat, HOST> FloatVariant;
    FloatVariant floatVariant(uniform_size);
    floatVariant.set(node->getVariant());
    for (int i = 0; i < uniform_size; ++i)
      data[i] = floatVariant[i];
    values = data;
  }

  // TODO: Create a SH_GL_CHECK_ERROR that gets compiled out in release?
  switch (uniform_size) {
  case 1:    
    SH_GL_CHECK_ERROR(glUniform1fvARB(location, 1, values));
    break;
  case 2:
    SH_GL_CHECK_ERROR(glUniform2fvARB(location, 1, values));
    break;
  case 3:
    SH_GL_CHECK_ERROR(glUniform3fvARB(location, 1, values));
    break;
  case 4:
    SH_GL_CHECK_ERROR(glUniform4fvARB(location, 1, values));
    break;
  default:
    SH_DEBUG_ASSERT(0); // Unsupported size
  }
}

void GlslCode::update_int_uniform(const VariableNodePtr& node, const GLint location)
{
  const int uniform_size = node->size();

  // Space for temporary data if we need it during data conversion
  GLint data[4];
  const GLint *values;

  if (node->valueType() == SH_INT) {
    values = static_cast<const GLint *>(node->getVariant()->array());
  }
  else {
    // Componentwise cast to float and copy
    typedef DataVariant<GLint, HOST> IntVariant;
    IntVariant intVariant(uniform_size);
    intVariant.set(node->getVariant());
    for (int i = 0; i < uniform_size; ++i)
      data[i] = intVariant[i];
    values = data;
  }

  // TODO: Create a SH_GL_CHECK_ERROR that gets compiled out in release?
  switch (uniform_size) {
  case 1:
    SH_GL_CHECK_ERROR(glUniform1ivARB(location, 1, values));
    break;
  case 2:
    SH_GL_CHECK_ERROR(glUniform2ivARB(location, 1, values));
    break;
  case 3:
    SH_GL_CHECK_ERROR(glUniform3ivARB(location, 1, values));
    break;
  case 4:
    SH_GL_CHECK_ERROR(glUniform4ivARB(location, 1, values));
    break;
  default:
    SH_DEBUG_ASSERT(0); // Unsupported size
  }
}

void GlslCode::updateUniform(const VariableNodePtr& uniform)
{
  if (!m_bound) return;

  SH_DEBUG_ASSERT(uniform);

  // Variable is in the map?
  const GlslVariable *var;
  if (m_varmap->contains(uniform, var)) {
    if (!var->texture() && !var->builtin()) {
      real_update_uniform(uniform, var->name());
    }
  }
  else {
    // Check to see if the uniform was split
    SH::Transformer::VarSplitMap::iterator split = m_splits.find(uniform);
    if (split != m_splits.end()) {
      Transformer::VarNodeVec &splitVec = split->second;
      VariantCPtr uniformVariant = uniform->getVariant();

      int offset = 0;
      int copySwiz[4];
      for (Transformer::VarNodeVec::iterator it = splitVec.begin();
	         it != splitVec.end(); offset += (*it)->size(), ++it) {
	      // TODO switch to properly swizzled version
        for (int i=0; i < (*it)->size(); ++i) {
	        copySwiz[i] = i + offset;
	      }
        (*it)->setVariant(uniformVariant->get(false,
          Swizzle(uniform->size(), (*it)->size(), copySwiz))); 
        updateUniform(*it);
      }
    }
    return;
  }  
}

void GlslCode::real_update_uniform(const VariableNodePtr& uniform, const string& name)
{
  // TODO: cache these?
  GLint location = glGetUniformLocationARB(m_bound, name.c_str());
  if (location != -1) {
    if (isInteger(uniform->valueType())) {
      update_int_uniform(uniform, location);
    } else {
      update_float_uniform(uniform, location);
    }
  }
}

string GlslCode::print_decl(const GlslVariableDeclaration& decl)
{
  string s = decl.declaration + ";";
  if (!decl.sh_name.empty()) {
    s += " // " + decl.sh_name;
  }
  return s;
}

ostream& GlslCode::print(ostream& out)
{
  SH_DEBUG_ASSERT(m_varmap);
  const string BLOCK_INDENT("    ");

  out << "// OpenGL " << ((m_unit == GLSL_VP) ? "Vertex" : "Fragment") << " Program" << endl;

  // Declare extensions
  for (ExtensionMap::const_iterator i = m_glsl_extensions.begin();
       i != m_glsl_extensions.end(); ++i) {
    out << "#extension " << i->first << " : ";
    switch (i->second) {
      case EXT_DISABLE: out << "disable"; break;
      case EXT_WARN:    out << "warn";    break;
      case EXT_ENABLE:  out << "enable";  break;
      case EXT_REQUIRE: out << "require"; break;
    };
    out << endl;
  }
  out << endl;

  // Declare attributes
  if (m_varmap->attribute_begin() != m_varmap->attribute_end()) {
    for (GlslVariableMap::DeclarationList::const_iterator i = m_varmap->attribute_begin(); 
	 i != m_varmap->attribute_end(); i++) {
      out << "attribute " << print_decl(*i) << endl;
    }
    out << endl;
  }

  // Declare uniforms
  if (m_varmap->uniform_begin() != m_varmap->uniform_end()) {
    for (GlslVariableMap::DeclarationList::const_iterator i = m_varmap->uniform_begin(); 
	 i != m_varmap->uniform_end(); i++) {
      out << "uniform " << print_decl(*i) << endl;
    }
    out << endl;
  }

  out << "void main()" << endl;
  out << "{" << endl;

  // Declare and initialize temporaries and constants
  if (m_varmap->regular_begin() != m_varmap->regular_end()) {
    for (GlslVariableMap::DeclarationList::const_iterator i = m_varmap->regular_begin(); 
	 i != m_varmap->regular_end(); i++) {
      out << BLOCK_INDENT << print_decl(*i) << endl;
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

ostream& GlslCode::describe_bindings(ostream& out)
{
  SH_DEBUG_ASSERT(m_varmap);

  out << "Inputs:" << endl;
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_inputs(); 
       i != m_shader->end_inputs(); ++i) {
    VariableNodePtr var = *i;
    out << "  " << var->name() << " => " << m_varmap->variable(var).name() << endl;
  }

  out << "Outputs:" << endl;
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_outputs(); 
       i != m_shader->end_outputs(); ++i) {
    VariableNodePtr var = *i;
    out << "  " << var->name() << " => " << m_varmap->variable(var).name() << endl;
  }

  return out;
}

void GlslCode::use_extension(const std::string &ext, ExtBehaviour behaviour)
{
  ExtensionMap::iterator i = m_glsl_extensions.find(ext);
  if (i == m_glsl_extensions.end()) {
    m_glsl_extensions[ext] = behaviour;
  } else {
    i->second = std::max(i->second, behaviour);
  }
}

void GlslCode::append_line(const string& line, bool append_semicolon)
{
  GlslLine l;
  l.indent = m_indent;
  l.code = append_semicolon ? line + ";" : line;  
  m_lines.push_back(l);
}

void GlslCode::gen_structural_node(const StructuralNodePtr& node)
{
  if (!node) return;

  if (node->type == StructuralNode::UNREDUCED) {
    BasicBlockPtr block = node->cfg_node->block;
    if (block) for (BasicBlock::StmtList::const_iterator I = block->begin();
                    I != block->end(); ++I) {
      const Statement& stmt = *I;
      emit(stmt);
    }
  } 
  else if (node->type == StructuralNode::BLOCK) {
    for (StructuralNode::StructNodeList::const_iterator I = node->structnodes.begin();
         I != node->structnodes.end(); ++I) {
      gen_structural_node(*I);
    }
  } 
  else if (node->type == StructuralNode::IFELSE) {
    StructuralNodePtr header = node->structnodes.front();

    Variable cond;
    StructuralNodePtr ifnode, elsenode;
    for (StructuralNode::SuccessorList::iterator I = header->succs.begin();
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
  else if (node->type == StructuralNode::IF) {
    StructuralNodePtr header = node->structnodes.front();

    StructuralNode::SuccessorList::iterator B;
    Variable cond;
    StructuralNodePtr ifnode;
    for (B = header->succs.begin(); B != header->succs.end(); ++B) {
      if (B->first.node()) {
        cond = B->first;
        ifnode = B->second;
        break;
      }
    }

    gen_structural_node(header);
    append_line("if (bool(" + resolve(cond.node()) + ")) {", false);
    m_indent++;
    gen_structural_node(ifnode);
    m_indent--;
    append_line("} // if", false);
  } 
  else if (node->type == StructuralNode::ELSE) {
    StructuralNodePtr header = node->structnodes.front();

    StructuralNode::SuccessorList::iterator B;
    Variable cond;
    StructuralNodePtr elsenode;
    for (B = header->succs.begin(); B != header->succs.end(); ++B) {
      if (B->first.node()) cond = B->first;
      if (!B->first.node()) elsenode = B->second;
      if (cond.node() && elsenode) break;
    }

    gen_structural_node(header);
    append_line("if (!(bool(" + resolve(cond.node()) + "))) {", false);
    m_indent++;
    gen_structural_node(elsenode);
    m_indent--;
    append_line("} // if", false);
  }
  else if (node->type == StructuralNode::WHILELOOP) {
    StructuralNodePtr header = node->structnodes.front();
    Variable cond = header->succs.front().first;
    StructuralNodePtr body = node->structnodes.back();

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
  else if (node->type == StructuralNode::SELFLOOP) {
    StructuralNode* loopnode = node->structnodes.front();

    bool condexit = true; // true if the condition causes us to exit the
                          // loop, rather than continue it
    Variable cond;
    for (StructuralNode::SuccessorList::iterator I = loopnode->succs.begin();
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
  else if (StructuralNode::PROPINT == node->type) {
    append_line("// Multi-exit infinite loop", false);
    append_line("while (true) {", false);
    m_indent++;

    int continue_levels=0;
    StructuralNode* continue_destination=0;

    set<StructuralNode*> seen; // keep track of the nodes we have seen already
    StructuralNodePtr n = node->structnodes.front(); 
    while (seen.find(n.object()) == seen.end()) {
      // Close the continue block if necessary
      if (continue_destination && n.object() == continue_destination) {
        for (int i=0; i < continue_levels; i++) {
          m_indent--;
          append_line("}", false);
        }
        continue_levels = 0;
      }

      seen.insert(n.object());
      gen_structural_node(n);
      if (n->succs.size() > 1) {
        // Figure out which branch is the conditional one
        pair<Variable, StructuralNodePtr> branch_pair = n->succs.front(); // conditional successor
        pair<Variable, StructuralNodePtr> default_pair = n->succs.back(); // non-conditional successor
        if (!branch_pair.first.node()) {
          branch_pair = n->succs.back();
          default_pair = n->succs.front();
        }
        Variable branch_cond = branch_pair.first;
        const StructuralNodePtr branch_node = branch_pair.second;

        // Check if the non-conditional node leaves the PROPINT node
        if (default_pair.second == node->succs.front().second) {
          // We are on the condition of a "while (branch_cond)" loop
          // so we can negate the condition and break
          append_line("if (!bool(" + resolve(branch_cond.node()) + ")) break; // loop condition", false);
          n = branch_pair.second;
        } else {
          // We are on a node that looks like
          //   if (branch_cond) break/continue;
          if (branch_node == node->succs.front().second) {
            // 'break' case
            append_line("if (bool(" + resolve(branch_cond.node()) + ")) break");
          } else {
            // 'continue' case
            // Wrap the rest of the body (until continue_destination) in an if statement
            append_line("if (!bool(" + resolve(branch_cond.node()) + ")) {", false);
            m_indent++;
            continue_levels++;
            continue_destination = branch_node.object();
          }

          n = default_pair.second;
        }
      } else {
        n = n->succs.front().second; // only one successor
      }
    }

    // Close the continue block if it hasn't been done already
    for (int i=0; i < continue_levels; i++) {
      m_indent--;
      append_line("}", false);
    }

    m_indent--;
    append_line("} // infinite while loop", false);
  }
  else {
    SH_DEBUG_WARN("Unknown StructuralNode type encountered.  Generated code may be incomplete.");
  }
}

string GlslCode::resolve(const Variable& v, int index, int size) const
{
  SH_DEBUG_ASSERT(m_varmap);

  if ((0 == size) || (v.size() == size)) {
    return m_varmap->resolve(v, index);
  } else {
    return glsl_typename(v.valueType(), size) + "("  + m_varmap->resolve(v, index) + ")";
  }
}

string GlslCode::resolve(const Variable& v, const Variable& index) const
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
  for (ProgramNode::TexList::const_iterator i = m_shader->textures.begin();
       i != m_shader->textures.end(); i++) {
    TextureNodePtr node = *i;
    
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
  
  // TODO: Cache this result? We may want something like ArbLimits here (GlslLimits perhaps)
  GLint max_texture_units;
  SH_GL_CHECK_ERROR(glGetIntegerv((GLSL_VP == m_unit ?
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB : GL_MAX_TEXTURE_IMAGE_UNITS_ARB),
    &max_texture_units));

  // allocate remaining textures units with respect to the reserved list
  for (ProgramNode::TexList::const_iterator i = m_shader->textures.begin();
       i != m_shader->textures.end(); i++) {
    TextureNodePtr node = *i;
    
    if (node->meta("opengl:texunit").empty() &&
	      node->meta("opengl:preset").empty()) {
      GLuint index;      
      
      for (index = m_nb_textures; index < static_cast<GLuint>(max_texture_units); index++) {
	      if (find(reserved.begin(), reserved.end(), index) == reserved.end()) {
	        break;
	      }
      }
      
      if (index < static_cast<GLuint>(max_texture_units)) {
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
  
  for (ProgramNode::TexList::const_iterator i = m_shader->textures.begin();
       i != m_shader->textures.end(); i++) {
    TextureNodePtr texture = *i;

    if (m_texture_units.find(texture) == m_texture_units.end()) {
      cerr << "Texture '" << texture->name() << "' has no assigned unit." << endl;
      continue;
    }
    
    const GlslVariable& var(m_varmap->variable(shref_dynamic_cast<VariableNode>(texture)));
    GLint location = glGetUniformLocationARB(m_bound, var.name().c_str());

    if (location != -1) {
      int index = m_texture_units[texture].index;

      SH_GL_CHECK_ERROR(glUniform1iARB(location, index));
      
      if (!m_texture_units[texture].preset) {
	m_texture->bindTexture(texture, GL_TEXTURE0 + index, false);
      }
    } else {
      cerr << "Cannot find uniform texture named '" << var.name() << "'." << endl;
    }
  }
}

GlslSet* GlslCode::m_fallback_set = 0;

}
