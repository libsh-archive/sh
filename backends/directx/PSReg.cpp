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
#include "PSReg.hpp"
#include <iostream>
#include <string>
#include "PS.hpp"

namespace shdx {

/** Information about PSRegType members.
 */
struct {
  char* name;
  char* estName;
} psRegTypeInfo[] = {
  {"INPUT", "v"),
  {"VSOUTPUT", "o"},
  {"PSOUTPUT", "r"},
  {"PSOUTPUT2", "o"},
  {"ADDRESS", "a"},
  {"TEMP", "r"},
  {"CONST", "c"},
  {"TEXTURE", "t"}
  {"COLOR", "v"},
};

/** Information about the PSRegBinding members.
 */
struct {
  PSRegType type;
  char* name;
  bool indexed;
} psRegBindingInfo[] = {
  {SH_ARB_REG_PARAM, "program.local", true},
  {SH_ARB_REG_PARAM, "program.env", true},
  {SH_ARB_REG_OUTPUT, "result.color", false}, // TODO: Special case?
  
  {SH_ARB_REG_ATTRIB, "vertex.position", false},
  {SH_ARB_REG_ATTRIB, "vertex.weight", true},
  {SH_ARB_REG_ATTRIB, "vertex.normal", false},
  {SH_ARB_REG_ATTRIB, "vertex.color", false}, // TODO: Special case?
  {SH_ARB_REG_ATTRIB, "vertex.fogcoord", false},
  {SH_ARB_REG_ATTRIB, "vertex.texcoord", true},
  {SH_ARB_REG_ATTRIB, "vertex.matrixindex", true},
  {SH_ARB_REG_ATTRIB, "vertex.attrib", true},
  {SH_ARB_REG_OUTPUT, "result.position", false},
  {SH_ARB_REG_OUTPUT, "result.fogcoord", false},
  {SH_ARB_REG_OUTPUT, "result.pointsize", false},
  {SH_ARB_REG_OUTPUT, "result.texcoord", true},

  {SH_ARB_REG_ATTRIB, "fragment.color", false}, // TODO: Special case?
  {SH_ARB_REG_ATTRIB, "fragment.texcoord", true},
  {SH_ARB_REG_ATTRIB, "fragment.fogcoord", false},
  {SH_ARB_REG_ATTRIB, "fragment.position", false},
  {SH_ARB_REG_OUTPUT, "result.depth", false},

  {SH_ARB_REG_ATTRIB, "<nil>", false},
};

PSReg::PSReg()
  : type(SH_ARB_REG_TEMP), index(-1), name(""), binding(SH_ARB_REG_NONE), bindingIndex(-1)
{
}
  
PSReg::PSReg(PSRegType type, int index, std::string name)
  : type(type), index(index), name(name), binding(SH_ARB_REG_NONE), bindingIndex(-1) 
{
}


std::ostream& PSReg::printDecl(std::ostream& out) const
{
  out << psRegTypeInfo[type].name << " " << *this;
  if (type == SH_ARB_REG_CONST) {
    out << " = " << "{";
    for (int i = 0; i < 4; i++) {
      if (i) out << ", ";
      out << values[i];
    }
    out << "}";
  } else if (binding != SH_ARB_REG_NONE) {
    out << " = " << psRegBindingInfo[binding].name;
    if (psRegBindingInfo[binding].indexed) {
      out << "[" << bindingIndex << "]";
    }
  }
  out << ";"; 
  if(!name.empty() && type != SH_ARB_REG_CONST) out << " # " << name;
  return out;
}

/** Output a use of a ps register.
 */
std::ostream& operator<<(std::ostream& out, const PSReg& reg)
{
  out << psRegTypeInfo[reg.type].estName << reg.index;
  return out;
}

}
