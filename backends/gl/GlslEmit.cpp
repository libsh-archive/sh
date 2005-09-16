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
#include <cmath>

namespace shgl {

using namespace SH;
using namespace std;

static GlslMapping opCodeTable[] = {
  {SH_OP_ABS,   "abs($0)"},
  {SH_OP_ACOS,  "acos($0)"},
  {SH_OP_ADD,   "$0 + $1"},
  {SH_OP_ASIN,  "asin($0)"},
  {SH_OP_ASN,   "$0"},
  {SH_OP_ATAN,  "atan($0)"},
  {SH_OP_CEIL,  "ceil($0)"},
  {SH_OP_COS,   "cos($0)"},
  {SH_OP_DOT,   "dot($0, $1)"},
  {SH_OP_DIV,   "$0 / $1"},
  {SH_OP_DX,    "dFdx($0)"},
  {SH_OP_DY,    "dFdy($0)"},
  {SH_OP_EXP,   "exp($0)"},
  {SH_OP_EXP2,  "exp2($0)"},
  {SH_OP_FLR,   "floor($0)"},
  {SH_OP_FRAC,  "fract($0)"},
  {SH_OP_LOG,   "log($0)"},
  {SH_OP_LOG2,  "log2($0)"},
  {SH_OP_LRP,   "mix($2, $1, $0)"},
  {SH_OP_MAD,   "$0 * $1 + $2"},
  {SH_OP_MAX,   "max($0, $1)"},
  {SH_OP_MIN,   "min($0, $1)"},
  {SH_OP_MOD,   "mod($0, $1)"},
  {SH_OP_MUL,   "$0 * $1"},
  {SH_OP_NEG,   "-($0)"},
  {SH_OP_NORM,  "normalize($0)"},
  {SH_OP_POW,   "pow($0, $1)"},
  {SH_OP_RCP,   "1.0 / $0"},
  {SH_OP_RND,   "floor($0 + 0.5)"},
  {SH_OP_RSQ,   "inversesqrt($0)"},
  {SH_OP_SGN,   "sign($0)"},
  {SH_OP_SIN,   "sin($0)"},
  {SH_OP_SQRT,  "sqrt($0)"},
  {SH_OP_TAN,   "tan($0)"},
  {SH_OP_XPD,   "cross($0, $1)"},

  {SH_OPERATION_END,  0} 
};

typedef map<SH::ShOperation, GlslOpCodeVecs> GlslOpCodeMap;

GlslOpCodeVecs::GlslOpCodeVecs(const GlslMapping& mapping)
{
  string code(mapping.code);

  // Dices up the code string into references #i or $i to 
  // src variables and the code fragments between references. 
  string::size_type i, j;
  i = j = 0;
  for (; (j = code.find_first_of("$", i)) != string::npos;) {
    frag.push_back(code.substr(i, j - i));
    i = j + 1;
    j = code.find_first_not_of("012345689", i);
    index.push_back(atoi(code.substr(i, j - i).c_str()));
    i = j;
  }
  if (i == string::npos) {
    frag.push_back("");
  } else {
    frag.push_back(code.substr(i));
  }
}

void GlslCode::table_substitution(const ShStatement& stmt, GlslOpCodeVecs codeVecs)
{
  unsigned i=0;

  // find the size of the biggest parameter
  int param_size=0;
  for (i=0; i < codeVecs.index.size(); i++) {
    const ShVariable& src = stmt.src[codeVecs.index[i]];
    if (src.size() > param_size) {
      param_size = src.size();
    }
  }
  
  // print each parameter
  stringstream operation;
  for (i=0; i < codeVecs.index.size(); i++) { 
    const ShVariable& src = stmt.src[codeVecs.index[i]];
    operation << codeVecs.frag[i] << resolve(src, -1, param_size);
  }
  operation << codeVecs.frag[i]; // code fragment after the last variable

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

void GlslCode::emit(const ShStatement &stmt)
{
  static GlslOpCodeMap opCodeMap;

  // Lazily fill-in opCodeMap from the above table
  if (opCodeMap.empty()) {
    for (int i=0; opCodeTable[i].op != SH_OPERATION_END; i++) {
      opCodeMap[opCodeTable[i].op] = GlslOpCodeVecs(opCodeTable[i]);
    }
  }

  if(opCodeMap.find(stmt.op) != opCodeMap.end()) {
    // Handle ops in the table first  
    table_substitution(stmt, opCodeMap[stmt.op]);
  } 
  else {
    // Handle the rest of the operations
    switch(stmt.op) {
    case SH_OP_CBRT:
      emit_cbrt(stmt);
      break;
    case SH_OP_CMUL:
      emit_prod(stmt);
      break;
    case SH_OP_CSUM:
      emit_sum(stmt);
      break;
    case SH_OP_COND:
      emit_cond(stmt);
      break;
    case SH_OP_EXP10:
      emit_exp10(stmt);
      break;
    case SH_OP_KIL:
      emit_discard(stmt, "discard");
      break;
    case SH_OP_LIT:
      emit_lit(stmt);
      break;
    case SH_OP_LOG10:
      emit_log10(stmt);
      break;
    case SH_OP_NOISE:
      emit_noise(stmt);
      break;
    case SH_OP_PAL:
      emit_pal(stmt);
      break;
    case SH_OP_RET:
      emit_discard(stmt, "return");
      break;
    case SH_OP_SEQ:
    case SH_OP_SGE:
    case SH_OP_SGT:
    case SH_OP_SLE:
    case SH_OP_SLT:
    case SH_OP_SNE:
      emit_logic(stmt);
      break;
    case SH_OP_TEX:
    case SH_OP_TEXI:
      emit_texture(stmt);
      break;
    case SH_OP_COSH:
    case SH_OP_SINH:
    case SH_OP_TANH:
      emit_hyperbolic(stmt);
      break;
    case SH_OP_COMMENT:
      emit_comment(stmt);
      break;
    default:
      shError(ShException(string("Glsl Code: Unknown operation ") + opInfo[stmt.op].name));
      break;
    }
  }
}

ShVariableNodePtr GlslCode::allocate_constant(const ShStatement& stmt, double constant, int size) const
{
  if (size <= 0) size = stmt.dest.size(); // default size is size of destination variable

  const ShVariableNodePtr& dest_node = stmt.dest.node();
  ShVariableNode* node = new ShVariableNode(SH_CONST, size, dest_node->valueType(), dest_node->specialType());
  
  ShDataVariant<double>* variant = new ShDataVariant<double>(size, constant);
  node->setVariant(variant);

  ShVariableNodePtr node_ptr = ShPointer<ShVariableNode>(node);
  return node_ptr;
}

ShVariableNodePtr GlslCode::allocate_temp(const ShStatement& stmt, int size) const
{
  if (size <= 0) size = stmt.dest.size(); // default size is size of destination variable
  
  const ShVariableNodePtr& dest_node = stmt.dest.node();
  ShVariableNode* node = new ShVariableNode(SH_TEMP, size, dest_node->valueType(), dest_node->specialType());
  ShVariableNodePtr node_ptr = ShPointer<ShVariableNode>(node);
  return node_ptr;
}

string GlslCode::resolve_constant(double constant, const ShVariable& var, int size) const
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

void GlslCode::emit_cbrt(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_CBRT == stmt.op);

  ShVariable temp(allocate_constant(stmt, 1.0 / 3.0));
  emit_pow(stmt.dest, stmt.src[0], temp);
}

void GlslCode::emit_comment(const ShStatement& stmt)
{
  append_line("// " + stmt.get_info<ShInfoComment>()->comment, false);
}

void GlslCode::emit_cond(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_COND == stmt.op);
  static string vendor((char*)glGetString(GL_VENDOR));

  const int size = stmt.src[0].size();
  if (("NVIDIA Corporation" == vendor) || (1 == size)) {
    // CG has a component-wise COND operator
    append_line(resolve(stmt.dest) + " = (" + resolve(stmt.src[0]) + " > " + 
		resolve_constant(0, stmt.src[0]) + ") ? " + resolve(stmt.src[1]) +
		" : " + resolve(stmt.src[2]));
  } else {
    // The Glsl spec requires the first argument to COND to be a scalar
    for (int i=0; i < size; i++) {
      append_line(resolve(stmt.dest, i) + " = (" + resolve(stmt.src[0], i) + " > " + 
		  resolve_constant(0, stmt.src[0], 1) + ") ? " + resolve(stmt.src[1], i) +
		  " : " + resolve(stmt.src[2], i));
    }
  }
}

void GlslCode::emit_discard(const ShStatement& stmt, const string& function)
{
  SH_DEBUG_ASSERT((SH_OP_KIL == stmt.op) || (SH_OP_RET == stmt.op));

  append_line(string("if (any(") + resolve(stmt.src[0]) + ")) " + function);
}

void GlslCode::emit_pow(const ShVariable& dest, const ShVariable& a, const ShVariable& b)
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

void GlslCode::emit_exp10(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_EXP10 == stmt.op);
  ShVariable c(allocate_constant(stmt, 1.0/std::log10(2.0)));
  append_line(resolve(stmt.dest) + " = exp2(" + resolve(c) + " * " + resolve(stmt.src[0]) + ")");
}

void GlslCode::emit_hyperbolic(const ShStatement& stmt)
{
  ShVariable two(allocate_constant(stmt, 2));
  ShVariable e(allocate_constant(stmt, M_E));

  ShVariable e_plusX(allocate_temp(stmt));
  ShVariable e_minusX(allocate_temp(stmt));
  emit_pow(e_plusX, e, stmt.src[0]);
  append_line(resolve(e_minusX) + " = pow(" + resolve(e) + ", -(" + resolve(stmt.src[0]) + "))");

  switch (stmt.op) {
  case SH_OP_COSH:
    // cosh x = [e^x + e^-x] / 2
    append_line(resolve(stmt.dest) + " = (" + resolve(e_plusX) + " + " + resolve(e_minusX) + ") / " + resolve(two));
    break;
  case SH_OP_SINH:
    // sinh x = [e^x - e^-x] / 2
    append_line(resolve(stmt.dest) + " = (" + resolve(e_plusX) + " - " + resolve(e_minusX) + ") / " + resolve(two));
    break;
  case SH_OP_TANH:
    // tanh x = sinh x / cosh x = [e^x - e^-x] / [e^x + e^-x]
    append_line(resolve(stmt.dest) + " = (" + resolve(e_plusX) + " - " + resolve(e_minusX) + ") / " +
                                        "(" + resolve(e_plusX) + " + " + resolve(e_minusX) + ")");
    break;
  default:
    SH_DEBUG_ASSERT(0);
  }
}

void GlslCode::emit_lit(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_LIT == stmt.op);
  
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

void GlslCode::emit_log10(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_LOG10 == stmt.op);
  ShVariable c(allocate_constant(stmt, std::log10(2.0))); 
  append_line(resolve(stmt.dest) + " = log2(" + resolve(stmt.src[0]) + ") * " + resolve(c) + "");
}

void GlslCode::emit_logic(const ShStatement& stmt)
{
  GlslMapping mapping;
  mapping.op = stmt.op;

  if ((stmt.src[0].size() > 1) || (stmt.src[1].size() > 1) ) {
    switch (stmt.op) {
    case SH_OP_SEQ:
      mapping.code = "equal($0, $1)";
      break;
    case SH_OP_SGT:
      mapping.code = "greaterThan($0, $1)";
      break;
    case SH_OP_SGE:
      mapping.code = "greaterThanEqual($0, $1)";
      break;
    case SH_OP_SNE:
      mapping.code = "notEqual($0, $1)";
      break;
    case SH_OP_SLT:
      mapping.code = "lessThan($0, $1)";
      break;
    case SH_OP_SLE:
      mapping.code = "lessThanEqual($0, $1)";
      break;
    default:
      SH_DEBUG_ASSERT(0);
    }
  } else {
    switch (stmt.op) {
    case SH_OP_SEQ:
      mapping.code = "$0 == $1";
      break;
    case SH_OP_SGT:
      mapping.code = "$0 > $1";
      break;
    case SH_OP_SGE:
      mapping.code = "$0 >= $1";
      break;
    case SH_OP_SNE:
      mapping.code = "$0 != $1";
      break;
    case SH_OP_SLT:
      mapping.code = "$0 < $1";
      break;
    case SH_OP_SLE:
      mapping.code = "$0 <= $1";
      break;
    default:
      SH_DEBUG_ASSERT(0);
    }
  }

  // TODO: cache these mappings?
  
  table_substitution(stmt, mapping);
}

void GlslCode::emit_noise(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_NOISE == stmt.op);

  int output_size = stmt.dest.size();
  SH_DEBUG_ASSERT((output_size <= 4) && (output_size >= 1));
  
  stringstream line;
  line << resolve(stmt.dest) << " = " << "noise" << output_size << "("
       << resolve(stmt.src[0]) << ")";
  append_line(line.str());
}

void GlslCode::emit_pal(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_PAL == stmt.op);

  append_line(resolve(stmt.dest) + " = " + resolve(stmt.src[0], stmt.src[1]));
}

void GlslCode::emit_prod(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_CMUL == stmt.op);

  ShVariable temp(allocate_temp(stmt, 1));
  append_line(resolve(temp) + " = " + resolve(stmt.src[0], 0));

  int size = stmt.src[0].size();
  for (int i=1; i < size; i++) {
    append_line(resolve(temp) + " *= " + resolve(stmt.src[0], i));
  }

  append_line(resolve(stmt.dest) + " = " + resolve(temp));
}

void GlslCode::emit_sum(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_CSUM == stmt.op);

  ShVariable temp(allocate_temp(stmt, 1));
  append_line(resolve(temp) + " = " + resolve(stmt.src[0], 0));

  int size = stmt.src[0].size();
  for (int i=1; i < size; i++) {
    append_line(resolve(temp) + " += " + resolve(stmt.src[0], i));
  }

  append_line(resolve(stmt.dest) + " = " + resolve(temp));
}

void GlslCode::emit_texture(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT((SH_OP_TEX == stmt.op) || (SH_OP_TEXI == stmt.op));

  stringstream line;
  line << resolve(stmt.dest) << " = texture";

  ShTextureNodePtr texture = shref_dynamic_cast<ShTextureNode>(stmt.src[0].node());
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
    line << "Cube";
    break;
  case SH_TEXTURE_RECT:
    line << "2DRect"; // Not supported on ATI, but there is no equivalent
    break;
  }

  line << "(" << resolve(stmt.src[0]) << ", " << resolve(stmt.src[1]) << ")";
  if (2 == texture->size()) {
    line << ".xw"; // 2-component inputs are uploaded as GL_LUMINANCE_ALPHA
  } else if (texture->size() != 4) {
    line << ".";
    for (int i = 0; i < texture->size(); i++) {
      line << "xyzw"[i];
    }
  }

  append_line(line.str());
}

}
