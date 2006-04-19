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
#include <cmath>

namespace shgl {

using namespace SH;
using namespace std;

static GlslMapping opCodeTable[] = {
  {OP_ABS,   "abs($0)"},
  {OP_ACOS,  "acos($0)"},
  {OP_ADD,   "$0 + $1"},
  {OP_ASIN,  "asin($0)"},
  {OP_ASN,   "$0"},
  {OP_ATAN,  "atan($0)"},
  {OP_CEIL,  "ceil($0)"},
  {OP_COS,   "cos($0)"},
  {OP_DOT,   "dot($0, $1)"},
  {OP_DIV,   "$0 / $1"},
  {OP_DX,    "dFdx($0)"},
  {OP_DY,    "dFdy($0)"},
  {OP_EXP,   "exp($0)"},
  {OP_EXP2,  "exp2($0)"},
  {OP_FLR,   "floor($0)"},
  {OP_FRAC,  "fract($0)"},
  //{OP_LOG,   "log($0)"}, // Needs ATI work-around
  {OP_LOG2,  "log2($0)"},
  {OP_LRP,   "mix($2, $1, $0)"},
  {OP_MAD,   "$0 * $1 + $2"},
  {OP_MAX,   "max($0, $1)"},
  {OP_MIN,   "min($0, $1)"},
  {OP_MOD,   "mod($0, $1)"},
  {OP_MUL,   "$0 * $1"},
  {OP_NEG,   "-($0)"},
  {OP_NORM,  "normalize($0)"},
  {OP_POW,   "pow($0, $1)"},
  {OP_RCP,   "1.0 / $0"},
  {OP_RND,   "floor($0 + 0.5)"},
  {OP_RSQ,   "inversesqrt($0)"},
  {OP_SGN,   "sign($0)"},
  {OP_SIN,   "sin($0)"},
  {OP_SQRT,  "sqrt($0)"},
  {OP_TAN,   "tan($0)"},
  {OP_XPD,   "cross($0, $1)"},

  {OPERATION_END,  0} 
};

void GlslCode::table_substitution(const Statement& stmt, const char* code_ptr)
{
  // TODO: This should definitely not be done every time a statement
  // is emitted, we should cache it under the operation type.

  string code = code_ptr;
  stringstream operation;
  int param_size=0;

  {
    // find the size of the biggest parameter
    string::size_type i = 0;
    for (string::size_type j=0; (j = code.find_first_of("$", i)) != string::npos; i = j) {
      i = j + 1;
      j = code.find_first_not_of("012345689", i);
      int index = atoi(code.substr(i, j - i).c_str());
      const Variable& src = stmt.src[index];
      if (src.size() > param_size) {
        param_size = src.size();
      }
    }
  }

  // Dices up the code string into references #i or $i to 
  // src variables and the code fragments between references.
  {
    string::size_type i = 0;
    for (string::size_type j=0; (j = code.find_first_of("$", i)) != string::npos; i = j) {
      // print fragment before the variable
      operation << code.substr(i, j - i);
      i = j + 1;
      j = code.find_first_not_of("012345689", i);

      // print the variable
      int index = atoi(code.substr(i, j - i).c_str());
      const Variable& src = stmt.src[index];
      operation << resolve(src, -1, param_size);
    }

    if (i != string::npos) {
      operation << code.substr(i);
    }
  }

  // create the full line of code
  stringstream line;
  line << resolve(stmt.dest) << " = ";

  if (param_size != stmt.dest.size()) {
    // cast for the destination size  
    line << glsl_typename(stmt.dest.valueType(), stmt.dest.size()); 
    line << "(" << operation.str() << ")";
  } else {
    line << operation.str();
  }

  append_line(line.str());
}

bool GlslCode::handle_table_operation(const Statement& stmt)
{
  const char* code = 0;
  {
    unsigned i = 0;
    for (Operation op = opCodeTable[i].op; op != OPERATION_END; op = opCodeTable[++i].op) {
      if (stmt.op == op) {
        code = opCodeTable[i].code;
      }
    }
  }

  if (code) {
    table_substitution(stmt, code);
    return true; // mapping was found
  } else {
    return false; // did not find a mapping in the table
  }
}

void GlslCode::emit(const Statement &stmt)
{
  if (!handle_table_operation(stmt)) {
    // Handle the rest of the operations
    switch(stmt.op) {
    case OP_CBRT:
      emit_cbrt(stmt);
      break;
    case OP_CMUL:
      emit_prod(stmt);
      break;
    case OP_CSUM:
      emit_sum(stmt);
      break;
    case OP_COND:
      emit_cond(stmt);
      break;
    case OP_EXP10:
      emit_exp10(stmt);
      break;
    case OP_KIL:
      emit_discard(stmt, "discard");
      break;
    case OP_LIT:
      emit_lit(stmt);
      break;
    case OP_LOG:
      emit_log(stmt);
      break;
    case OP_LOG10:
      emit_log10(stmt);
      break;
    case OP_NOISE:
      emit_noise(stmt);
      break;
    case OP_PAL:
      emit_pal(stmt);
      break;
    case OP_RET:
      emit_discard(stmt, "return");
      break;
    case OP_SEQ:
    case OP_SGE:
    case OP_SGT:
    case OP_SLE:
    case OP_SLT:
    case OP_SNE:
      emit_logic(stmt);
      break;
    case OP_TEX:
    case OP_TEXI:
    case OP_TEXLOD:
    case OP_TEXD:
      emit_texture(stmt);
      break;
    case OP_COSH:
    case OP_SINH:
    case OP_TANH:
      emit_hyperbolic(stmt);
      break;
    case OP_COMMENT:
      emit_comment(stmt);
      break;
    default:
      error(BackendException(string("Glsl Code: Unknown operation ") + opInfo[stmt.op].name));
      break;
    }
  }
}

VariableNodePtr GlslCode::allocate_constant(const Statement& stmt, double constant, int size) const
{
  if (size <= 0) size = stmt.dest.size(); // default size is size of destination variable

  const VariableNodePtr& dest_node = stmt.dest.node();
  VariableNode* node = new VariableNode(SH_CONST, size, dest_node->valueType(), dest_node->specialType());
  
  DataVariant<double>* variant = new DataVariant<double>(size, constant);
  VariantCPtr variant_ptr = variant;
  node->setVariant(variant_ptr);

  VariableNodePtr node_ptr = Pointer<VariableNode>(node);
  return node_ptr;
}

VariableNodePtr GlslCode::allocate_temp(const Statement& stmt, int size) const
{
  if (size <= 0) size = stmt.dest.size(); // default size is size of destination variable
  
  const VariableNodePtr& dest_node = stmt.dest.node();
  VariableNode* node = new VariableNode(SH_TEMP, size, dest_node->valueType(), dest_node->specialType());
  VariableNodePtr node_ptr = Pointer<VariableNode>(node);
  return node_ptr;
}

string GlslCode::resolve_constant(double constant, const Variable& var, int size) const
{
  if (size <= 0) size = var.size(); // default size is size of variable

  stringstream s;
  s.precision(16);
  s << glsl_typename(var.valueType(), size);

  s << "(";
  for (int i=0; i < size; i++) {
    if (i > 0) s << ", ";
    s << constant;
  }
  s << ")";

  return s.str();
}

void GlslCode::emit_ati_workaround(const Statement& stmt, double real_answer, const char* code)
{
  if (m_vendor == VENDOR_ATI) {
    Variable tmp(allocate_temp(stmt));
    Variable constant(allocate_constant(stmt, real_answer));
    table_substitution(Statement(tmp, stmt.src[0], stmt.op, stmt.src[1]), code);
    append_line(resolve(stmt.dest) + " = (" + resolve(stmt.src[0]) + " == " + resolve(stmt.src[1]) + ") ? " + resolve(constant) + " : " + resolve(tmp));
  } else {
    table_substitution(stmt, code);
  }
}

void GlslCode::emit_cbrt(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_CBRT == stmt.op);

  Variable temp(allocate_constant(stmt, 1.0 / 3.0));
  emit_pow(stmt.dest, stmt.src[0], temp);
}

void GlslCode::emit_comment(const Statement& stmt)
{
  append_line("// " + stmt.get_info<InfoComment>()->comment, false);
}

void GlslCode::emit_cond(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_COND == stmt.op);

  bool on_nvidia = false;
  const GLubyte* vendor = GL_CHECK_ERROR(glGetString(GL_VENDOR));
  if (vendor) {
    string s(reinterpret_cast<const char*>(vendor));
    on_nvidia = s.find("NVIDIA") != s.npos;
  }

  const int size = stmt.src[0].size();
  if (on_nvidia || (1 == size)) {
    // CG has a component-wise COND operator
    string code = string ("(") + resolve(stmt.src[0]) + " > " + 
      resolve_constant(0, stmt.src[0]) + ") ? " + resolve(stmt.src[1]) +
      " : " + resolve(stmt.src[2]);
  
    if (size != stmt.dest.size()) {
      code = glsl_typename(stmt.dest.valueType(), stmt.dest.size()) +  "(" + code + ")";
    }
    append_line(resolve(stmt.dest) + " = " + code);
  } else {
    // The Glsl spec requires the first argument to COND to be a scalar
    for (int i=0; i < size; i++) {
      append_line(resolve(stmt.dest, i) + " = (" + resolve(stmt.src[0], i) + " > " + 
		  resolve_constant(0, stmt.src[0], 1) + ") ? " + resolve(stmt.src[1], i) +
		  " : " + resolve(stmt.src[2], i));
    }
  }
}

void GlslCode::emit_discard(const Statement& stmt, const string& function)
{
  SH_DEBUG_ASSERT((OP_KIL == stmt.op) || (OP_RET == stmt.op));

  // Form a statement consistent with Sh's "discard" semantics
  ostringstream oss;
  oss << "if (";  
  if (stmt.src[0].size() > 1) {
    // Vector operand - component wise compare and use "any"
    oss << "any(greaterThan(";
    oss << resolve(stmt.src[0]);
    oss << ", ";
    oss << resolve_constant(0, stmt.src[0]);
    oss << "))";
  } else {
    // Scalar operand - discard of greater than 0
    oss << resolve(stmt.src[0]);
    oss << " > float(0)";
  }
  // Close "if" and output contents
  oss << ") " << function;

  // Add the line
  append_line(oss.str());
}

void GlslCode::emit_pow(const Variable& dest, const Variable& a, const Variable& b)
{
  // cast each argument's size if necessary
  const int size = std::max(a.size(), b.size());
  string pow_call = string("pow(") + resolve(a, -1, size) + ", " + resolve(b, -1, size) + ")";
  
  // cast to the destination size if necessary
  if (dest.size() != size) {
    pow_call = glsl_typename(dest.valueType(), dest.size()) + "(" + pow_call + ")";
  }

  append_line(resolve(dest) + " = " + pow_call);
}

void GlslCode::emit_exp10(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_EXP10 == stmt.op);
  Variable c(allocate_constant(stmt, 1.0/std::log10(2.0)));
  append_line(resolve(stmt.dest) + " = exp2(" + resolve(c) + " * " + resolve(stmt.src[0]) + ")");
}

void GlslCode::emit_hyperbolic(const Statement& stmt)
{
  Variable two(allocate_constant(stmt, 2));
  Variable e(allocate_constant(stmt, M_E));

  Variable e_plusX(allocate_temp(stmt));
  Variable e_minusX(allocate_temp(stmt));
  emit_pow(e_plusX, e, stmt.src[0]);
  append_line(resolve(e_minusX) + " = pow(" + resolve(e) + ", -(" + resolve(stmt.src[0]) + "))");

  switch (stmt.op) {
  case OP_COSH:
    // cosh x = [e^x + e^-x] / 2
    append_line(resolve(stmt.dest) + " = (" + resolve(e_plusX) + " + " + resolve(e_minusX) + ") / " + resolve(two));
    break;
  case OP_SINH:
    // sinh x = [e^x - e^-x] / 2
    append_line(resolve(stmt.dest) + " = (" + resolve(e_plusX) + " - " + resolve(e_minusX) + ") / " + resolve(two));
    break;
  case OP_TANH:
    // tanh x = sinh x / cosh x = [e^x - e^-x] / [e^x + e^-x]
    append_line(resolve(stmt.dest) + " = (" + resolve(e_plusX) + " - " + resolve(e_minusX) + ") / " +
                                        "(" + resolve(e_plusX) + " + " + resolve(e_minusX) + ")");
    break;
  default:
    SH_DEBUG_ASSERT(0);
  }
}

void GlslCode::emit_lit(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_LIT == stmt.op);
  
  // Result according to OpenGL spec
  append_line(resolve(stmt.dest, 0) + " = " + resolve_constant(1, stmt.dest, 1));

  append_line(resolve(stmt.dest, 1) + " = max(" + resolve_constant(0, stmt.dest, 1) +
	      ", " + resolve(stmt.src[0], 0) + ")");

  append_line(resolve(stmt.dest, 2) + " = " + resolve(stmt.src[0], 0) + 
	      " > " + resolve_constant(0, stmt.src[0], 1) + " ? pow(max(" + 
	      resolve_constant(0, stmt.dest, 1) + ", " + resolve(stmt.src[0], 1) + 
	      "), clamp(" + resolve(stmt.src[0], 2) + ", " + 
	      resolve_constant(-128, stmt.dest, 1) + ", " + 
	      resolve_constant(128, stmt.dest, 1) + ")) : " + resolve_constant(0, stmt.dest, 1));

  append_line(resolve(stmt.dest, 3) + " = " + resolve_constant(1, stmt.dest, 1));
}

void GlslCode::emit_log(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_LOG == stmt.op);
  bool on_ati = false;
  const GLubyte* vendor = GL_CHECK_ERROR(glGetString(GL_VENDOR));
  if (vendor) {
    std::string s(reinterpret_cast<const char*>(vendor));
    on_ati = s.find("ATI") != s.npos;
  }

  if (on_ati) {
    Variable tmp(allocate_temp(stmt));
    table_substitution(Statement(tmp, stmt.op, stmt.src[0]), "log($0)");

    const int size = stmt.dest.size();
    if (1 == size) {
      // Must be handled separately to preserve the destination swizzle
      append_line(resolve(stmt.dest) + " = (" + resolve(stmt.src[0]) + " == " + 
                  resolve_constant(1, stmt.src[0], 1) + ") ? " + resolve_constant(0, stmt.src[0], 1) +
                  " : " + resolve(tmp, 0));
    } else {
      for (int i=0; i < size; i++) {
        append_line(resolve(stmt.dest, i) + " = (" + resolve(stmt.src[0], i) + " == " + 
                    resolve_constant(1, stmt.src[0], 1) + ") ? " + resolve_constant(0, stmt.src[0], 1) +
                    " : " + resolve(tmp, i));
      }
    }
  } else {
    table_substitution(stmt, "log($0)");
  }
}

void GlslCode::emit_log10(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_LOG10 == stmt.op);
  Variable c(allocate_constant(stmt, std::log10(2.0))); 
  append_line(resolve(stmt.dest) + " = log2(" + resolve(stmt.src[0]) + ") * " + resolve(c) + "");
}

void GlslCode::emit_logic(const Statement& stmt)
{
  string code = "";
  if ((stmt.src[0].size() > 1) || (stmt.src[1].size() > 1) ) {
    switch (stmt.op) {
    case OP_SEQ:
      code = "equal($0, $1)";
      break;
    case OP_SGT:
      code = "greaterThan($0, $1)";
      break;
    case OP_SGE:
      code = "greaterThanEqual($0, $1)";
      break;
    case OP_SNE:
      code = "notEqual($0, $1)";
      break;
    case OP_SLT:
      code = "lessThan($0, $1)";
      break;
    case OP_SLE:
      code = "lessThanEqual($0, $1)";
      break;
    default:
      SH_DEBUG_ASSERT(0);
    }
  } else {
    switch (stmt.op) {
    case OP_SEQ:
      code = "$0 == $1";
      break;
    case OP_SGT:
      code = "$0 > $1";
      break;
    case OP_SGE:
      code = "$0 >= $1";
      break;
    case OP_SNE:
      code = "$0 != $1";
      break;
    case OP_SLT:
      code = "$0 < $1";
      break;
    case OP_SLE:
      code = "$0 <= $1";
      break;
    default:
      SH_DEBUG_ASSERT(0);
    }
  }

  // cast for the destination type
  code = glsl_typename(stmt.dest.valueType(), stmt.dest.size()) +  "(" + code + ")";

  table_substitution(stmt, code.c_str());
}

void GlslCode::emit_noise(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_NOISE == stmt.op);

  int output_size = stmt.dest.size();
  SH_DEBUG_ASSERT((output_size <= 4) && (output_size >= 1));
  
  stringstream line;
  line << resolve(stmt.dest) << " = " << "noise" << output_size << "("
       << resolve(stmt.src[0]) << ")";
  append_line(line.str());
}

void GlslCode::emit_pal(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_PAL == stmt.op);

  append_line(resolve(stmt.dest) + " = " + resolve(stmt.src[0], stmt.src[1]));
}

void GlslCode::emit_prod(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_CMUL == stmt.op);

  Variable temp(allocate_temp(stmt, 1));
  append_line(resolve(temp) + " = " + resolve(stmt.src[0], 0));

  int size = stmt.src[0].size();
  for (int i=1; i < size; i++) {
    append_line(resolve(temp) + " *= " + resolve(stmt.src[0], i));
  }

  append_line(resolve(stmt.dest) + " = " + resolve(temp));
}

void GlslCode::emit_sum(const Statement& stmt)
{
  SH_DEBUG_ASSERT(OP_CSUM == stmt.op);

  Variable temp(allocate_temp(stmt, 1));
  append_line(resolve(temp) + " = " + resolve(stmt.src[0], 0));

  int size = stmt.src[0].size();
  for (int i=1; i < size; i++) {
    append_line(resolve(temp) + " += " + resolve(stmt.src[0], i));
  }

  append_line(resolve(stmt.dest) + " = " + resolve(temp));
}

void GlslCode::emit_texture(const Statement& stmt)
{
  SH_DEBUG_ASSERT((OP_TEX    == stmt.op) || (OP_TEXI == stmt.op) ||
                  (OP_TEXLOD == stmt.op) || (OP_TEXD == stmt.op));

  SH::Operation op = stmt.op; 
  bool cg_function = false;
  bool ati_function = false;
  if (OP_TEXD == stmt.op) {    
    switch (m_vendor) {
      // On NVIDIA, for TEXD we can use the Cg functions (tex2D, etc.)
      case VENDOR_NVIDIA:
        cg_function = true;
        break;

      // On ATI, we can use the extension
      case VENDOR_ATI:
        ati_function = true;
        use_extension("GL_ATI_shader_texture_lod", EXT_REQUIRE);
        break;

      // On other hardware, we're sunk, so just ignore it
      default:
        op = OP_TEX;
        SH_DEBUG_WARN("TEXD is not supported on this hardware in the GLSL backend");
        break;
    }
  }

  stringstream line;
  line << resolve(stmt.dest) << (cg_function ? " = tex" : " = texture");

  TextureNodePtr texture = shref_dynamic_cast<TextureNode>(stmt.src[0].node());
  switch (texture->dims()) {
  case SH_TEXTURE_1D:
    line << "1D";
    break;
  case SH_TEXTURE_2D:
    line << "2D";
    break;
  case SH_TEXTURE_3D:
    line << "3D";
    break;
  case SH_TEXTURE_CUBE:
    line << (cg_function ? "CUBE" : "Cube");
    break;
  case SH_TEXTURE_RECT:
    // Not supported on ATI, but there is no equivalent
    line << (cg_function ? "RECT" : "2DRect");
    break;
  }

  if (OP_TEXLOD == stmt.op) {
    // TODO: Technically this requires an extension... enable it?
    line << "Lod";
  } else if (ati_function) {
    line << "_ATI";
  }

  line << "(" << resolve(stmt.src[0]) << ", " << resolve(stmt.src[1]);

  if (OP_TEXLOD == stmt.op) {
    line << ", " << resolve(stmt.src[2]);
  } else if (OP_TEXD == stmt.op) {
    // TODO: Support derivative lookup for 3D and CUBE textures (i.e. float3 derivatives)
    line << ", " << resolve(stmt.src[2](0,1)) << ", " << resolve(stmt.src[2](2,3));
  }

  line << ")";

  // Apply the right swizzle, based on the texture size and destination size
  if ((2 == texture->size()) || (stmt.dest.size() != 4)) {
    string components = "xyzw";
    if (2 == texture->size()) {
      components = "xwyz"; // 2-component inputs are uploaded as GL_LUMINANCE_ALPHA
    }
    
    line << ".";
    const int swiz_length = stmt.dest.size();
    for (int i = 0; i < swiz_length; i++) {
      line << components[i];
    }
  }

  append_line(line.str());
}

}
