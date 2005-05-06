// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ArbReg.hpp"
#include <iostream>
#include <string>
#include "Arb.hpp"

namespace shgl {

/** Information about ArbRegType members.
 */
static struct {
  char* name;
  char* estName;
} arbRegTypeInfo[] = {
  {"ATTRIB", "i"},
  {"PARAM", "u"},
  {"TEMP", "t"},
  {"HALFTEMP", "h"},
  {"ADDRESS", "a"},
  {"OUTPUT", "o"},
  {"PARAM", "c"},
  {"<texture>", "<texture>"}
};

/** Information about the ArbRegBinding members.
 */
static struct {
  ArbRegType type;
  char* name;
  bool indexed;
} arbRegBindingInfo[] = {
  {SH_ARB_REG_PARAM, "program.local", true},
  {SH_ARB_REG_PARAM, "program.env", true},
  {SH_ARB_REG_PARAM, "<nil>", true},
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

ArbReg::ArbReg()
  : type(SH_ARB_REG_TEMP), index(-1), preset(false), name("")
{
    binding.type = SH_ARB_REG_NONE;
    binding.index = -1;
    binding.count = 1;
}
  
ArbReg::ArbReg(ArbRegType type, int index, std::string name)
  : type(type), index(index), preset(false), name(name)
{
    binding.type = SH_ARB_REG_NONE;
    binding.index = -1;
    binding.count = 1;
}


std::ostream& ArbReg::printDecl(std::ostream& out) const
{
  out << arbRegTypeInfo[type].name << " " << *this;
  if (type == SH_ARB_REG_CONST) {
    out << " = " << "{";
    for (int i = 0; i < 4; i++) {
      if (i) out << ", ";
      out << binding.values[i];
    }
    out << "}";
  } else if (binding.type != SH_ARB_REG_NONE) {
  if (binding.type == SH_ARB_REG_STATE)
    {
    out << " = " << binding.name;
    }
  else
    {
    if (binding.count > 1) {
      out << "[" << binding.count << "]";
    }
    out << " = ";
    if (binding.count > 1) out << "{ ";
    out << arbRegBindingInfo[binding.type].name;
    if (arbRegBindingInfo[binding.type].indexed) {
      out << "[" << binding.index;
      if (binding.count > 1) {
        out << " .. " << (binding.index + binding.count - 1);
      }
      out << "]";
    }
    if (binding.count > 1) out << " }";
    }
  }
  out << ";"; 
  if(!name.empty() && type != SH_ARB_REG_CONST) out << " # " << name;
  return out;
}

/** Output a use of an arb register.
 */
std::ostream& operator<<(std::ostream& out, const ArbReg& reg)
{
  out << arbRegTypeInfo[reg.type].estName << reg.index;
  return out;
}

}
