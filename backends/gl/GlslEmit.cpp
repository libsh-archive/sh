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

namespace shgl {

using namespace SH;
using namespace std;

struct GlslMapping {
  ShOperation op;
  const char* code;
};

static GlslMapping opCodeTable[] = {
  {SH_OP_ASN,   "$0"},
  {SH_OP_ADD,   "$0 + $1"},
  {SH_OP_COND,  "$0 ? $1 : $2"},
  {SH_OP_DOT,   "dot($0, $1)"},
  {SH_OP_MAX,   "max($0, $1)"},
  {SH_OP_MUL,   "$0 * $1"},
  {SH_OP_NORM,  "normalize($0)"},
  {SH_OP_POW,   "pow($0, $1)"},

  {SH_OPERATION_END,  0} 
};

struct GlslOpCodeVecs
{
  GlslOpCodeVecs(const GlslMapping& mapping);

  GlslOpCodeVecs() {}
  bool operator<(const GlslOpCodeVecs &other) {
    return op < other.op;
  }

  ShOperation op;
  vector<int> index;
  vector<string> frag;
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

void GlslCode::emit(const ShStatement &stmt)
{
  static GlslOpCodeMap opCodeMap;

  // Lazily fill-in opCodeMap from the above table
  if (opCodeMap.empty()) {
    for (int i=0; opCodeTable[i].op != SH_OPERATION_END; i++) {
      opCodeMap[opCodeTable[i].op] = GlslOpCodeVecs(opCodeTable[i]);
    }
  }

  stringstream line;
  line << resolve(stmt.dest) << " = ";
  
  if(opCodeMap.find(stmt.op) != opCodeMap.end()) {
    // Handle ops in the table first  
    GlslOpCodeVecs codeVecs = opCodeMap[stmt.op];

    unsigned i;
    for (i=0; i < codeVecs.index.size(); i++) { 
      const ShVariable& src = stmt.src[codeVecs.index[i]];
      line << codeVecs.frag[i] << resolve(src);
    }

    line << codeVecs.frag[i]; // code fragment after the last variable
  } 
  else {
    // Handle the rest of the operations
    switch(stmt.op) {
    case SH_OP_TEX:
    case SH_OP_TEXI:
      line << emit_texture(stmt);
      break;
    default:
      line.str("// *** unhandled operation " + string(opInfo[stmt.op].name) + " ***");
      break;
    }
  }

  m_lines.push_back(line.str());
}

string GlslCode::emit_texture(const ShStatement &stmt)
{
  SH_DEBUG_ASSERT((SH_OP_TEX == stmt.op) || (SH_OP_TEXI == stmt.op));

  stringstream line;
  line << "texture";

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

  return line.str();
}

}
