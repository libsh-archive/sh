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
  //{SH_OP_ATAN2, "atan($1 / $0)"}, // FIXME
  {SH_OP_CBRT,  "pow($0, 1.0f/3.0f)"},
  {SH_OP_CEIL,  "ceil($0)"},
  {SH_OP_COND,  "$0 ? $1 : $2"},
  {SH_OP_COS,   "cos($0)"},
  {SH_OP_DOT,   "dot($0, $1)"},
  {SH_OP_DIV,   "$0 / $1"},
  {SH_OP_DX,    "dFdx($0)"},
  {SH_OP_DY,    "dFdy($0)"},
  {SH_OP_EXP2,  "exp2($0)"},
  {SH_OP_FLR,   "floor($0)"},
  {SH_OP_FRAC,  "fract($0)"},
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
  {SH_OP_RCP,   "1.0f / $0"},
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
  unsigned i, j;
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
  stringstream line;
  line << resolve(stmt.dest) << " = ";
  
  unsigned i;
  for (i=0; i < codeVecs.index.size(); i++) { 
    const ShVariable& src = stmt.src[codeVecs.index[i]];
    line << codeVecs.frag[i] << resolve(src);
  }

  line << codeVecs.frag[i]; // code fragment after the last variable
    
  m_lines.push_back(line.str());
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
    case SH_OP_SEQ:
    case SH_OP_SGT:
    case SH_OP_SGE:
    case SH_OP_SNE:
    case SH_OP_SLT:
    case SH_OP_SLE:
      emit_logic(stmt);
      break;
    case SH_OP_CMUL:
      emit_prod(stmt);
      break;
    case SH_OP_CSUM:
      emit_sum(stmt);
      break;
    case SH_OP_EXP:
      emit_exp(stmt, M_E);
      break;
    case SH_OP_EXP10:
      emit_exp(stmt, 10);
      break;
    case SH_OP_KIL:
      emit_discard(stmt);
      break;
    case SH_OP_LIT:
      emit_lit(stmt);
      break;
    case SH_OP_LOG:
      emit_log(stmt, M_E);
      break;
    case SH_OP_LOG10:
      emit_log(stmt, 10);
      break;
    case SH_OP_TEX:
    case SH_OP_TEXI:
      emit_texture(stmt);
      break;
    default:
      shError(ShException(string("Glsl Code: Unknown operation ") + opInfo[stmt.op].name));
      break;
    }
  }
}

ShVariableNodePtr GlslCode::allocate_temp(const ShStatement& stmt) const
{
  // allocate a temporary variable based on the destination variable
  const ShVariableNodePtr& dest_node = stmt.dest.node();
  ShVariableNode* node = new ShVariableNode(dest_node->kind(), dest_node->size(), 
					    dest_node->valueType(), dest_node->specialType());
  ShVariableNodePtr node_ptr = ShPointer<ShVariableNode>(node);
  return node_ptr;
}

void GlslCode::emit_exp(const ShStatement& stmt, double power)
{
  SH_DEBUG_ASSERT((SH_OP_EXP == stmt.op) || (SH_OP_EXP10 == stmt.op));

  ShVariable temp(allocate_temp(stmt));
  stringstream s;
  s << power;
  
  m_lines.push_back(resolve(temp) + " = " + s.str());
  m_lines.push_back(resolve(stmt.dest) + " = pow(" + resolve(temp) + ", " + resolve(stmt.src[0]) + ")");
}

void GlslCode::emit_discard(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT((SH_OP_KIL == stmt.op));

  m_lines.push_back(std::string("if (") + resolve(stmt.src[0]) + ") discard;");
}

void GlslCode::emit_lit(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_LIT == stmt.op);

  // Result according to OpenGL spec
  m_lines.push_back(resolve(stmt.dest, 0) + " = 1.0f");

  m_lines.push_back(resolve(stmt.dest, 1) + " = max(0.0, " + resolve(stmt.src[0], 0) + ")");

  m_lines.push_back(resolve(stmt.dest, 2) + " = " + resolve(stmt.src[0], 0) + 
		    " > 0 ? pow(max(0.0, " + resolve(stmt.src[0], 1) + 
		    "), clamp(" + resolve(stmt.src[0], 2) + 
		    ", -128.0f, 128.0f)) : 0.0");

  m_lines.push_back(resolve(stmt.dest, 3) + " = 1.0f");
}

void GlslCode::emit_log(const ShStatement& stmt, double base)
{
  SH_DEBUG_ASSERT((SH_OP_LOG == stmt.op) || (SH_OP_LOG10 == stmt.op));

  const double log2_base = log(base) / log(2.0);

  ShVariable temp(allocate_temp(stmt)); 
  stringstream s;
  s << log2_base;

  m_lines.push_back(resolve(temp) + " = " + s.str());
  m_lines.push_back(resolve(stmt.dest) + " = log2(" + resolve(stmt.src[0]) + ") / " + resolve(temp) + "");
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
      mapping.code = "not(equal($0, $1))"; // notEqual() doesn't work on NVIDIA
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
      mapping.code = "($0 == $1)";
      break;
    case SH_OP_SGT:
      mapping.code = "($0 > $1)";
      break;
    case SH_OP_SGE:
      mapping.code = "($0 >= $1)";
      break;
    case SH_OP_SNE:
      mapping.code = "($0 != $1)";
      break;
    case SH_OP_SLT:
      mapping.code = "($0 < $1)";
      break;
    case SH_OP_SLE:
      mapping.code = "($0 <= $1)";
      break;
    default:
      SH_DEBUG_ASSERT(0);
    }
  }

  // TODO: cache these mappings

  table_substitution(stmt, mapping);
}

void GlslCode::emit_prod(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_CMUL == stmt.op);

  ShVariable temp(allocate_temp(stmt));
  m_lines.push_back(resolve(temp) + " = 1");

  int size = stmt.src[0].size();
  for (int i=0; i < size; i++) {
    m_lines.push_back(resolve(temp) + " *= " + resolve(stmt.src[0], i));
  }

  m_lines.push_back(resolve(stmt.dest) + " = " + resolve(temp));
}

void GlslCode::emit_sum(const ShStatement& stmt)
{
  SH_DEBUG_ASSERT(SH_OP_CSUM == stmt.op);

  ShVariable temp(allocate_temp(stmt));
  m_lines.push_back(resolve(temp) + " = 0");

  int size = stmt.src[0].size();
  for (int i=0; i < size; i++) {
    m_lines.push_back(resolve(temp) + " += " + resolve(stmt.src[0], i));
  }

  m_lines.push_back(resolve(stmt.dest) + " = " + resolve(temp));
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
    line << "2DRect";
    break;
  }

  line << "(" << resolve(stmt.src[0]) << ", " << resolve(stmt.src[1]) << ")";

  m_lines.push_back(line.str());
}

}